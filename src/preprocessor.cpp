#include "preprocessor.h"
#include "file_utils.h"
#include "macro_utils.h"

#include <iostream>
#include <sstream>
#include <vector>


/**
 * Ersetzt rekursiv alle \include{...}-Anweisungen durch den Inhalt
 * der jeweils referenzierten Datei.
 *
 * Die Verarbeitung erfolgt zeilenbasiert auf SourceLine-Ebene, sodass
 * Dateiname und originale Zeilennummern erhalten bleiben.
 *
 * Semantik:
 *   \include{file.tex}
 * wird ersetzt durch:
 *   [Inhalt von file.tex]
 *
 * Fehler (z. B. Syntaxfehler oder fehlende Dateien) werden im
 * PreprocReport gesammelt und abbrechfrei behandelt.
 *
 *
 * @param content        Eingabetext als Liste von SourceLine
 * @param report         Zentrale Fehler- und Warnungssammlung
 * @param include_stack  Aktueller Include-Pfad (zur Zyklenerkennung)
 * @return               Neuer SourceLine-Vektor mit aufgelösten Includes
 */
std::vector<SourceLine> process_include(const std::vector<SourceLine>& content,
    PreprocReport& report,
    std::unordered_set<std::string>& include_stack
)
{
    std::vector<SourceLine> result;

    for (const SourceLine& sl : content) {

        // Führende Whitespaces entfernen (für \include-Erkennung)
        std::string trimmed = sl.line;
        trimmed.erase(0, trimmed.find_first_not_of(" \t"));

        // Keine Include-Zeile → unverändert übernehmen
        if (!trimmed.starts_with("\\include{")) {
            result.push_back(sl);
            continue;
        }

        // Klammern finden
        size_t open = trimmed.find('{');
        size_t close = trimmed.find('}', open + 1);

        if (open == std::string::npos || close == std::string::npos) {
            report.errors.push_back({
                sl.file,
                "Syntaxfehler in \\include: fehlende geschweifte Klammern",
                sl.line_nr
            });
            result.push_back(sl);
            continue;
        }

        // Dateiname extrahieren
        std::string filename =
            trimmed.substr(open + 1, close - open - 1);

        if (filename.empty()) {
            report.errors.push_back({
                sl.file,
                "\\include: Dateiname ist leer",
                sl.line_nr
            });
            result.push_back(sl);
            continue;
        }

        // Zyklische Includes erkennen
        if (include_stack.contains(filename)) {
            report.errors.push_back({
                sl.file,
                "Zyklisches \\include entdeckt: " + filename,
                sl.line_nr
            });
            result.push_back(sl);
            continue;
        }

        // Datei lesen
        std::vector<SourceLine> included = read_file_lines(filename);

        if (included.empty()) {
            report.errors.push_back({
                sl.file,
                "Include-Datei konnte nicht gelesen werden: " + filename,
                sl.line_nr
            });
            result.push_back(sl);
            continue;
        }

        // Rekursion mit Stack-Schutz
        include_stack.insert(filename);
        included = process_include(included, report, include_stack);
        include_stack.erase(filename);

        // Inhalt einfügen
        result.insert(result.end(), included.begin(), included.end());
    }

    return result;
}



/**
 * Extrahiert alle \define-Makros aus dem LaTeX-Quelltext.
 *
 * Beispiele:
 *   \define{AUTHOR}{Max}   -> "AUTHOR" -> "Max"
 *   \define{DEBUG}         -> "DEBUG" -> ""
 * 
 * Parameter:
 *      content – Der vollständige Eingabetext, gespeichert in einem Vector vom Typ-Struct. Jedes Struct enthält eine Zeile, den Namen der Datei und die Zeilennummer. 
 * Rückgabe:
 *      Eine HashMap (unordered_map), die alle gefundenen Makros enthält.
 */
std::unordered_map<std::string, std::string> extract_defines(const std::vector<SourceLine>& content, PreprocReport& report)
{
    std::unordered_map<std::string, std::string> macros;

    for (const SourceLine& sl : content) {

        // Führende Leerzeichen entfernen
        std::string trimmed = sl.line;
        trimmed.erase(0, trimmed.find_first_not_of(" \t"));

        // Keine Define-Zeile
        if (!trimmed.starts_with("\\define")) {
            continue;
        }

        // Muss mit \define{ beginnen
        if (!trimmed.starts_with("\\define{")) {
            report.errors.push_back({
                sl.file,
                "Syntaxfehler: Erwartet \\define{KEY}{...}",
                sl.line_nr
            });
            continue;
        }

        // KEY extrahieren
        size_t key_open = trimmed.find('{');
        size_t key_close = trimmed.find('}', key_open + 1);

        if (key_open == std::string::npos || key_close == std::string::npos) {
            report.errors.push_back({
                sl.file,
                "Syntaxfehler: \\define ohne korrekt geschlossenen KEY",
                sl.line_nr
            });
            continue;
        }

        std::string key =
            trimmed.substr(key_open + 1, key_close - key_open - 1);



        // KEY darf keine geschweiften Klammern enthalten
        if (key.find('{') != std::string::npos ||
            key.find('}') != std::string::npos) {
            report.errors.push_back({
                sl.file,
                "Syntaxfehler: Ungültiger Makro-Name in \\define (verschachtelte Klammern)",
                sl.line_nr
            });
            continue;
        }

        if (key.empty()) {
            report.errors.push_back({
                sl.file,
                "Syntaxfehler: KEY darf nicht leer sein",
                sl.line_nr
            });
            continue;
        }

        // ggf. VALUE extrahieren 
        std::string value;
        size_t pos = key_close + 1;

        // Bounds-Check
        if (pos < trimmed.size() && trimmed[pos] == '{') {

            size_t val_open = pos;
            size_t val_close = trimmed.find('}', val_open + 1);

            if (val_close == std::string::npos) {
                report.errors.push_back({
                    sl.file,
                    "Syntaxfehler: Unvollständige Value-Klammern in \\define",
                    sl.line_nr
                });
                continue;
            }

            value = trimmed.substr(
                val_open + 1,
                val_close - val_open - 1
            );
        }
        // else: kein Value → value bleibt ""

        // Doppelte Keys
        if (macros.contains(key)) {
            std::cout << "Warnung: Makro '" + key + "' wird überschrieben" << "\n";
        }

        macros[key] = value;
    } 
    
    return macros;
}


/**
 * Entfernt alle syntaktisch erkannten \define{...}-Anweisungen aus dem Quelltext.
 *
 * Die Funktion arbeitet zeilenbasiert auf einer Liste von SourceLine-Objekten
 * und entfernt ausschließlich Zeilen, die nach Entfernen führender Leerzeichen,
 * mit "\define{" beginnen.
 *
 * Es findet keine Syntaxprüfung statt; fehlerhafte \define-Anweisungen werden
 * unverändert beibehalten. Die eigentliche Validierung erfolgt in extract_defines().
 *
 * Parameter:
 *   content - Vektor von SourceLine-Strukturen (Text, Datei, originale Zeilennummer)
 *
 * Rückgabe:
 *   Neuer Vektor von SourceLine ohne \define-Zeilen.
 *   Die ursprünglichen Dateinamen und Zeilennummern bleiben erhalten.
 */
std::vector<SourceLine> remove_defines(const std::vector<SourceLine>& content) {
    std::vector<SourceLine> result;
   
    for (const SourceLine& sl : content) {

        // Führende Whitespaces entfernen (Indentierung ignorieren)
        std::string trimmed = sl.line;
        trimmed.erase(0, trimmed.find_first_not_of(" \t"));

        // Nur echte \define{...}-Zeilen entfernen
        if (!trimmed.starts_with("\\define{")) {
            result.push_back(sl);
        }
    }

    return result;
}



namespace {

    /**
     * Prüft, ob ein Zeichen Teil eines Bezeichners ist.
     *
     * Als Bezeichnerzeichen gelten:
     *  - Buchstaben (A–Z, a–z)
     *  - Ziffern (0–9)
     *  - Unterstrich (_)
     *
     * Diese Definition wird verwendet, um Wortgrenzen zu erkennen
     * und Teilersetzungen (z. B. AUTHOR in AUTHOR_NAME) zu vermeiden.
     */
    bool is_ident_char(unsigned char c) {
        return std::isalnum(c) || c == '_';
    }

} // anonymer Namespace


/**
 * Ersetzt im Text alle Makronamen durch ihre zugehörigen Werte aus der \define-Tabelle.
 *
 * Dabei werden ausschließlich exakte Bezeichner ersetzt. Ein Makro wird nur dann
 * ersetzt, wenn links und rechts keine weiteren Bezeichnerzeichen angrenzen.
 *
 * Beispiel:
 *   Text:    "AUTHOR_NAME ist AUTHOR."
 *   Defines: {"AUTHOR" -> "Max"}
 *   Ergebnis: "AUTHOR_NAME ist Max."
 *
 * Parameter:
 *   text    – Eingabetext als Vektor von SourceLine-Strukturen
 *   macros  – HashMap mit \define-Makros (Key -> Value)
 *
 * Rückgabe:
 *   Neuer Vektor von SourceLine mit ersetzten Makros
 */
std::vector<SourceLine> replace_text_macros(
    const std::vector<SourceLine>& text,
    const std::unordered_map<std::string, std::string>& macros)
{
    // Kopie, damit Originaldaten erhalten bleiben
    std::vector<SourceLine> result = text;

    // Für jedes definierte Makro
    for (const auto& [key, value] : macros) {

        if (key.empty()) {
            continue;
        }

        // Jede Zeile separat verarbeiten
        for (SourceLine& sl : result) {

            size_t pos = 0;

            // Alle Vorkommen des Makros in der Zeile finden
            while ((pos = sl.line.find(key, pos)) != std::string::npos) {

                // Linke Wortgrenze prüfen
                bool left_ok =
                    (pos == 0) ||
                    !is_ident_char(static_cast<unsigned char>(sl.line[pos - 1]));

                // Rechte Wortgrenze prüfen
                size_t right_index = pos + key.size();
                bool right_ok =
                    (right_index >= sl.line.size()) ||
                    !is_ident_char(static_cast<unsigned char>(sl.line[right_index]));

                if (left_ok && right_ok) {
                    // Exakter Treffer → ersetzen
                    sl.line.replace(pos, key.size(), value);
                    pos += value.size(); // hinter die Ersetzung springen
                }
                else {
                    // Kein exakter Treffer → weiter suchen
                    pos += key.size();
                }
            }
        }
    }

    return result;
}


/**
 * Verarbeitet \ifdef-Blöcke mit optionalem \else.
 *
 * Einschränkungen:
 * - Keine Verschachtelung von \ifdef-Blöcken erlaubt
 * - Bedingungen prüfen ausschließlich auf Existenz in `defines`
 *
 * Parameter:
 *   text     – Quelltext als Liste von SourceLine (inkl. Datei & Zeilennummer)
 *   defines  – zuvor extrahierte \define-Makros
 *   report   – Fehler- und Warnungssammlung
 *
 * Rückgabe:
 *   Gefilterter Text mit entfernten/aktivierten Ifdef-Blöcken
 */
std::vector<SourceLine> process_conditionals(
    const std::vector<SourceLine>& text,
    const std::unordered_map<std::string, std::string>& defines,
    PreprocReport& report
) {
    std::vector<SourceLine> result;

    bool inside_if_block = false;
    bool skip_if_block = false;

    int if_start_line = -1;
    std::string if_start_file;

    for (const SourceLine& sl : text) {

        // Führende Whitespaces entfernen (Direktiven tolerant erkennen)
        std::string trimmed = sl.line;
        trimmed.erase(0, trimmed.find_first_not_of(" \t"));

        // ---------- \ifdef ----------
        if (trimmed.starts_with("\\ifdef{")) {

            if (inside_if_block) {
                report.errors.push_back({
                    sl.file,
                    "Verschachtelte \\ifdef-Blöcke werden nicht unterstützt",
                    sl.line_nr
                });
                continue;
            }

            size_t open = trimmed.find('{');
            size_t close = trimmed.find('}', open + 1);

            // Ungültige oder leere Bedingung
            if (open == std::string::npos || close == std::string::npos || close <= open + 1) {
                report.errors.push_back({
                    sl.file,
                    "Syntaxfehler in \\ifdef: Erwartet \\ifdef{NAME}",
                    sl.line_nr
                });
                result.push_back(sl);
                continue;
            }

            std::string macro = trimmed.substr(open + 1, close - open - 1);

            inside_if_block = true;
            skip_if_block = (defines.find(macro) == defines.end());

            if_start_line = sl.line_nr;
            if_start_file = sl.file;
            continue;
        }

        // ---------- \else ----------
        if (trimmed == "\\else") {

            if (!inside_if_block) {
                report.errors.push_back({
                    sl.file,
                    "\\else ohne vorheriges \\ifdef",
                    sl.line_nr
                });
                continue;
            }

            skip_if_block = !skip_if_block;
            continue;
        }

        // ---------- \endif ----------
        if (trimmed == "\\endif") {

            if (!inside_if_block) {
                report.errors.push_back({
                    sl.file,
                    "\\endif ohne vorheriges \\ifdef",
                    sl.line_nr
                });
            }

            inside_if_block = false;
            skip_if_block = false;
            if_start_line = -1;
            if_start_file.clear();
            continue;
        }

        // ---------- Normale Zeilen ----------
        if (!inside_if_block || !skip_if_block) {
            result.push_back(sl);
        }
    }

    // Fehlendes \endif am Dateiende
    if (inside_if_block) {
        report.errors.push_back({
            if_start_file,
            "Fehlendes \\endif für \\ifdef (Beginn in Zeile " +
            std::to_string(if_start_line) + ")",
            if_start_line
        });
    }

    return result;
}






