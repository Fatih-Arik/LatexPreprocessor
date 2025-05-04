#include "preprocessor.h"
#include "file_utils.h"
#include "macro_utils.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <vector>


 

/**
 * Ersetzt alle `#include "filename"`-Anweisungen im Text durch den tatsächlichen Inhalt der referenzierten Datei.
 *
 * Die Funktion durchsucht den Text zeilenweise nach Include-Direktiven und ersetzt sie mit dem Inhalt
 * der jeweils referenzierten Datei. Dabei werden nur gültige Zeilen berücksichtigt,
 * die dem Muster `#include "pfad/zur/datei.tex"` entsprechen.
 *
 * Parameter:
 *     content – Der vollständige LaTeX-Text, in dem Includes verarbeitet werden sollen.
 *
 * Rückgabe:
 *     Ein neuer Text, in dem alle `#include`-Anweisungen ersetzt wurden.
 */

std::string process_include(const std::string& content) {


    // #include "(.*?)"
    // #include -> Sucht nach dem Schlüsselwort `#include`
    // " -> Erwartet ein doppeltes Anführungszeichen (escaped als \" in C++)
    // (.*?) -> Greift den Dateinamen ab:
    //   . → Beliebiges Zeichen (auch Leerzeichen)
    //   * -> Beliebig viele Zeichen (einschließlich keiner)
    //   ? -> Minimal greedy: Nimmt nur so viele Zeichen, bis das nächste `"` erscheint
    // " -> Erwartet das schließende Anführungszeichen
    std::regex include_regex("#include \"(.*?)\"");
    std::stringstream result;
    std::istringstream stream(content);
    std::string line;
    std::string include_file;          
    std::string included_content;

    while (std::getline(stream, line)) {

        std::smatch match;
        if (std::regex_search(line, match, include_regex)) {    // regex_search prüft ob ein Teil des Strings übereinstimmt, damit wir #include innerhalb eines Textes verwenden können
            include_file = match[1].str();          // Extrahierter Dateiname
            included_content = read_file(include_file);
            result << included_content << "\n";
        }
        else {
            result << line << "\n";
        }
    }
    return result.str();
}


/**
 * Extrahiert alle `#define`-Makros aus dem Quelltext und speichert sie als Schlüssel-Wert-Paare.
 *
 * Ein Makro muss im Format `#define NAME WERT` vorliegen.
 * Zeilen, die diesem Muster entsprechen, werden analysiert und gesammelt.
 *
 * Beispiel:
 *     #define AUTHOR Max Mustermann
 *     → Makro["AUTHOR"] = "Max Mustermann"
 *
 * Parameter:
 *     content – Der vollständige Eingabetext, in dem nach Makros gesucht wird.
 *
 * Rückgabe:
 *     Eine HashMap (unordered_map), die alle gefundenen Makros enthält.
 */
std::unordered_map<std::string, std::string> extract_defines(const std::string& content) {
    // Regex zur Erkennung von `#define NAME Wert`
    // #define\s+(\w+)\s+(.+)
    // #define -> Schlüsselwort `#define`
    // \s+ -> Mindestens ein Leerzeichen
    // (\w+) -> Der Makroname (nur Buchstaben/Zahlen/Unterstriche)
    // \s+ -> Mindestens ein Leerzeichen zwischen Name und Wert
    // (.+) -> Der Wert des Makros (alles nach dem Namen)

    std::regex define_regex("#define\\s+(\\w+)\\s+(.*)");
    std::unordered_map<std::string, std::string> macros;
    std::istringstream stream(content); 

    std::string line; 


    // Lese den Text Zeile für Zeile
    while (std::getline(stream, line)) {
        std::smatch match; // Speichert das Ergebnis der Regex-Suche
        std::string key = match[1].str();               // Extrahiert den Makro-Namen (z. B. "AUTHOR")
        std::string value = match[2].str();             // Extrahiert den Makro-Wert (z. B. "Max Mustermann")

        // Prüft, ob die aktuelle Zeile der `#define`-Syntax entspricht
        if (std::regex_match(line, match, define_regex)) {  // Der gesamte String muss bei regex_match übereinstimmen
            key = match[1].str();               // Extrahiert den Makro-Namen (z. B. "AUTHOR")
            value = match[2].str();             // Extrahiert den Makro-Wert (z. B. "Max Mustermann")

            // Speichert das Makro in der HashMap (Key-Value-Paar)
            macros[key] = value;
        }
    }

    return macros; // Gibt die HashMap mit allen gefundenen Makros zurück
}


/**
 * Ersetzt im Text alle Makronamen durch ihre zugehörigen Werte aus der Makro-Tabelle.
 *
 * Jeder Eintrag im Makro-Dictionary (z. B. "NAME" → "Max Mustermann") wird im gesamten Text
 * durch den entsprechenden Wert ersetzt – allerdings nur bei exakten Wortübereinstimmungen.
 *
 * Beispiel:
 *     Vorher: "Mein Name ist NAME."
 *     Makro:  {"NAME": "Max Mustermann"}
 *     Nachher: "Mein Name ist Max Mustermann."
 *
 * Parameter:
 *     text    – Der zu bearbeitende Eingabetext.
 *     macros  – Eine HashMap mit allen definierten Makros und ihren Ersetzungen.
 *
 * Rückgabe:
 *     Der verarbeitete Text mit allen Makro-Ersetzungen.
 */
std::string replace_text_macros(const std::string& text, const std::unordered_map<std::string, std::string>& macros) {

    std::string result = text;
    // \b stellt sicher, dass nur ganze Wörter ersetzt werden
    for (const auto& macro : macros) {
        std::regex macro_regex("\\b" + macro.first + "\\b");
        result = std::regex_replace(result, macro_regex, macro.second);
    }
    return result;
}

/**
 * Entfernt alle `#define`-Makros aus dem Text sowie die zugehörigen Leerzeilen.
 *
 * Dabei wird jede Zeile, die mit `#define` beginnt, durch ein Platzhalter-Marker ersetzt
 * und anschließend vollständig entfernt. So bleiben keine unnötigen Leerzeilen im Text zurück.
 *
 * Parameter:
 *     text – Referenz auf den LaTeX-Text, aus dem Makros entfernt werden sollen.
 *
 * Rückgabe:
 *     Der bereinigte Text ohne `#define`-Makros und leere Makro-Zeilen.
 */
std::string remove_defines(std::string& text) {
    // 1. Ersetze alle #define-Zeilen durch einen Platzhalter
    // ^          Zeilenanfang
    // #define    Das wörtliche Schlüsselwort
    // .*         Beliebige Zeichen bis zum Ende der Zeile
    // $          Zeilenende
    std::regex macro_regex(R"(^#define.*$)");
    text = std::regex_replace(text, macro_regex, "+HIDDEN+");

    // 2. Entferne alle Zeilen, die nur aus dem Marker + evtl. Leerzeichen + \n bestehen
    // ^              Zeilenanfang
    // \+HIDDEN\+     Der Marker
    // \s*            Optional: Leerzeichen oder Tabs
    // \n             Zeilenumbruch
    std::regex empty_line_regex(R"(^\+HIDDEN\+\s*\n)");
    text = std::regex_replace(text, empty_line_regex, "");

    return text;
}



/**
 * Führt alle bekannten Makrovereinfachungen im Text aus.
 *
 * Diese Funktion verarbeitet vordefinierte mathematische Makros wie `frac`, `sqrt`, `abs` usw.,
 * sowie Inline- und Block-Mathematik (#math, #blockmath) und Codeblöcke (#codeblock).
 *
 * Beispiel:
 *   Eingabe: #math(frac(1, sqrt(2))) → Ausgabe: \( \frac{1}{\sqrt{2}} \)
 *
 * Parameter:
 *   - input: Der ursprüngliche Text mit Makros.
 *
 * Rückgabe:
 *   - Der vereinfachte Text mit allen Makros ersetzt durch gültige LaTeX-Syntax.
 */
std::string simplify_all_macros(const std::string& input) {
    std::string result = input;
    std::vector<MacroSpec> known_macros = {
     {"frac", 2, "\\frac{__0__}{__1__}"},
     {"sqrt", 1, "\\sqrt{__0__}"},
     {"abs", 1, "\\left|__0__\\right|"},
     {"log", 1, "\\log{__0__}"},
     {"pow", 2, "{__0__}^{__1__}"}
    };

    for (const auto& spec : known_macros) {
        result = simplify_macro_spec(result, spec);
    }

    result = simplify_inline_math(result);
    result = simplify_block_math(result);
    result = simplify_codeblocks(result);
    return result;
}



/**
 * Extrahiert die Programmiersprache aus einem Codeblock-Makro wie z. B.:
 *     #codeblock[cpp]{...}
 *
 * Parameter:
 * - text:        Der vollständige Quelltext.
 * - start_pos:   Startposition des Makros im Text (z. B. Position von "#codeblock[").
 * - lang_end_pos: Rückgabeparameter – Position der schließenden eckigen Klammer `]`.
 *
 * Rückgabewert:
 * - Sprachname wie "cpp", "python", "java" usw.
 * - Gibt einen leeren String zurück, falls keine schließende Klammer `]` gefunden wurde.
 */
std::string extract_language(const std::string& text, size_t start_pos, size_t& lang_end_pos) {
    size_t lang_start = start_pos + std::string("#codeblock[").length(); // Position nach '['
    size_t lang_end = text.find(']', lang_start); // Suche nach schließender Klammer

    if (lang_end == std::string::npos) {
        lang_end_pos = lang_end;
        return "";  // Fehler: Kein ']' gefunden
    }
    lang_end_pos = lang_end;  // Rückgabe der Position von ']'
    return text.substr(lang_start, lang_end - lang_start); // Sprachname extrahieren
}




/**
 * Extrahiert den Inhalt eines Codeblocks aus einem Makro wie:
 *     #codeblock[cpp]{ ... }
 *
 * Parameter:
 * - text:       Der vollständige Eingabetext.
 * - start_pos:  Die Position der öffnenden geschweiften Klammer `{`.
 * - end_pos:    Referenz: Rückgabe der Position der passenden schließenden Klammer `}`.
 *
 * Rückgabewert:
 * - Der extrahierte Quellcode zwischen `{` und `}`.
 * - Gibt einen leeren String zurück, wenn keine passende schließende Klammer gefunden wurde.
 */
std::string extract_codeblock_body(const std::string& text, size_t start_pos, size_t& end_pos) {
    int brace_depth = 0;
    std::string code;
    bool in_block = false;

    for (size_t i = start_pos; i < text.size(); ++i) {
        char c = text[i];

        if (c == '{') {
            brace_depth++;
            if (!in_block) {
                in_block = true; // Start der Blockextraktion
                continue;        // Erste öffnende Klammer nicht miterfassen
            }
        }
        else if (c == '}') {
            brace_depth--;
            if (brace_depth == 0) {
                end_pos = i;     // Block endet
                return code;
            }
        }

        if (in_block) {
            code += c;
        }
    }

    // Falls keine passende schließende Klammer gefunden wurde
    end_pos = text.size();
    return "";
}


/**
 * Wandelt alle #codeblock[language]{...}-Makros in LaTeX \lstlisting-Umgebungen um.
 *
 * Beispiel:
 *     #codeblock[cpp]{int main() { return 0; }}
 *     → \begin{lstlisting}[language=cpp]
 *         int main() { return 0; }
 *       \end{lstlisting}
 *
 * Parameter:
 * - input: Der vollständige Eingabetext mit potenziellen Codeblöcken.
 *
 * Rückgabewert:
 * - Der Text, in dem alle Codeblock-Makros durch LaTeX-kompatible Listings ersetzt wurden.
 */
std::string simplify_codeblocks(const std::string& input) {
    
    std::string result = input;
    std::string code = "";
    std::string replacement = "";
    size_t pos = 0;
    size_t lang_end = 0;
    size_t brace_start = 0;
    size_t brace_end = 0; 


    while ((pos = result.find("#codeblock[", pos)) != std::string::npos) {

        std::string language = extract_language(result, pos, lang_end);
       
        if (lang_end == std::string::npos) {
            pos += 1;  // ungültige Syntax → weiter
            continue;
        }

    

        // Suchen nach der öffnenden geschweiften Klammer '{'
        brace_start = result.find('{', lang_end);
        if (brace_start == std::string::npos) {
            pos += 1;
            continue;
        }

        // Extrahiere den Quellcode-Block
        brace_end = 0;
        code = extract_codeblock_body(result, brace_start, brace_end);

        if (code.empty()) {
            pos += 1;  // Ungültiger oder unvollständiger Block
            continue;
        }

        // Latex-Befehl erzeugen
        replacement =
            "\\begin{lstlisting}[language=" + language + "]\n" +
            code + "\n\\end{lstlisting}";

        // Ersetzen des ursprünglichen Makros durch das LaTeX-Listing
        result.replace(pos, brace_end - pos + 1, replacement);

        // Weiter hinter dem eingefügten Block
        pos += replacement.length();
    }

    return result;
}


