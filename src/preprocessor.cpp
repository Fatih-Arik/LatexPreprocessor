#include "preprocessor.h"
#include "file_utils.h"
#include "macro_utils.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <vector>


 

/**
 * Ersetzt alle `\include{"filename"}`-Anweisungen im Text durch den tatsächlichen Inhalt der referenzierten Datei.
 *
 * Die Funktion durchsucht den Text zeilenweise nach Include-Direktiven und ersetzt sie mit dem Inhalt
 * der jeweils referenzierten Datei. Dabei werden nur gültige Zeilen berücksichtigt,
 * die dem Muster `\include{"pfad/zur/datei.tex"}` entsprechen.
 *
 * Parameter:
 *     content – Der vollständige LaTeX-Text, in dem Includes verarbeitet werden sollen.
 *
 * Rückgabe:
 *     Ein neuer Text, in dem alle `\include`-Anweisungen ersetzt wurden.
 */

std::string process_include(const std::string& content) {


    // \include\s*{\s*"(.*?)"\s*}
    // \include -> Sucht nach dem Schlüsselwort `\include`
    //    (.*?) -> Greift den Dateinamen ab:
    //      \s* -> Beliebig viele Leerzeichen sind erlaubt
    //        . -> Beliebiges Zeichen (auch Leerzeichen)
    //        * -> Beliebig viele Zeichen (einschließlich keiner)
    //        ? -> Minimal greedy: Nimmt nur so viele Zeichen, bis das nächste `"` erscheint
    std::regex include_regex(R"(\\include\s*\{\s*\"(.*?)\"\s*\})");

    std::stringstream result;
    std::istringstream stream(content);
    std::string line;
    std::string include_file;          
    std::string included_content;

  
    while (std::getline(stream, line)) {

        std::smatch match;
        if (std::regex_search(line, match, include_regex)) {    // regex_search prüft ob ein Teil des Strings übereinstimmt, damit wir \include innerhalb eines Textes verwenden können
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
 * Extrahiert alle `\define`-Makros aus dem Quelltext und speichert sie als Schlüssel-Wert-Paare.
 *
 * Ein Makro kann im Format `\define{KEY}` oder `\define{KEY}{WERT}` vorliegen.
 * Zeilen, die diesem Muster entsprechen, werden analysiert und gesammelt.
 *
 * 
 * Beispiel:
 *     \define{AUTHOR}{Max Mustermann} → Makro["AUTHOR"] = "Max Mustermann"
 *     \define{DEBUG}                  → Makro["DEBUG"]  = ""
 *
 * Parameter:
 *     content – Der vollständige Eingabetext, in dem nach Makros gesucht wird.
 *
 * Rückgabe:
 *     Eine HashMap (unordered_map), die alle gefundenen Makros enthält.
 */
std::unordered_map<std::string, std::string> extract_defines(const std::string& content) {
    // Regex zum Erfassen von Makros im Format:
    // \define{NAME}             → ohne Wert
    // \define{NAME}{WERT}       → mit Wert
    // (\w+)     = der Makroname (Buchstaben/Zahlen/_)
    // (?:...)   = non-capturing group, damit {...} optional ist
    // (.*)      = der Makro-Wert (alles innerhalb der zweiten Klammer)
    std::regex define_regex(R"(\\define\{(\w+)\}(?:\{(.*)\})?)");
    std::unordered_map<std::string, std::string> macros;
    std::istringstream stream(content); 

    std::string key;               
    std::string value;             
    std::string line; 

    // Zeile für Zeile durchgehen
    while (std::getline(stream, line)) {
        std::smatch match; 

        // Prüft, ob die Zeile zu unserem \define-Muster passt
        if (std::regex_match(line, match, define_regex)) {  
            key = match[1].str();                           // Der Makroname (z. B. "DEBUG", "AUTHOR")                  

            // Prüfe, ob ein Wert vorhanden war (zweites Gruppen-Match)
            if (match[2].matched) {
                value = match[2].str();        // Wert extrahieren
            }
            else {
                value = "";                    // Kein Wert → leerer String
            }
            macros[key] = value;
        }
    }

    return macros; // Gibt die HashMap mit allen gefundenen Makros zurück
}


/**
 * Ersetzt im Text alle Makronamen durch ihre zugehörigen Werte aus der Makro-Tabelle.
 *
 * Jeder Eintrag im Makro-Dictionary (z. B. "NAME" -> "Max Mustermann") wird im gesamten Text
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
 * Entfernt alle `\define`-Makros aus dem Text sowie die zugehörigen Leerzeilen.
 *
 * Dabei wird jede Zeile, die mit `\define` beginnt, durch ein Platzhalter-Marker ersetzt
 * und anschließend vollständig entfernt. So bleiben keine unnötigen Leerzeilen im Text zurück.
 *
 * Parameter:
 *     text – Referenz auf den LaTeX-Text, aus dem Makros entfernt werden sollen.
 *
 * Rückgabe:
 *     Der bereinigte Text ohne `\define`-Makros und leere Makro-Zeilen.
 */
std::string remove_defines(std::string& text) {
    // 1. Ersetze alle \define-Zeilen durch einen Platzhalter
    // ^          Zeilenanfang
    // \define    Das wörtliche Schlüsselwort
    // .*         Beliebige Zeichen bis zum Ende der Zeile
    // $          Zeilenende
    std::regex macro_regex(R"(^\\define.*$)");
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
 * Verarbeitet \ifdef-Blöcke mit optionalem \else.
 * Aktuell keine Verschachtelung.
 *
 * Parameter:
 *   text     – Eingabetext (LaTeX-ähnlich)
 *   defines  – Vorher definierte Makros (\define)
 *
 * Rückgabe:
 *   Gefilterter Text (nur gültige Teile der Ifdef-Blöcke bleiben erhalten)
 */
std::string process_conditionals(const std::string& text, const std::unordered_map<std::string, std::string>& defines) {
    std::istringstream stream(text);
    std::ostringstream output;
    std::string line;

    bool inside_if_block = false;    // Aktuell innerhalb eines \ifdef?
    bool skip_if_block = false;      // Soll dieser Block aktuell übersprungen werden?

    while (std::getline(stream, line)) {

        // Block-Beginn: \ifdef
        if (!inside_if_block && line.find(R"(\ifdef{)") != std::string::npos) {
            size_t start = line.find('{') + 1;
            size_t end = line.find('}', start);
            std::string current_macro = line.substr(start, end - start);

            inside_if_block = true;

            // Prüfe, ob das Makro definiert ist
            skip_if_block = defines.find(current_macro) == defines.end();
        }

        // Innerhalb eines Ifdef-Blocks: \else
        else if (inside_if_block && line.find(R"(\else)") != std::string::npos) {
            // Beim \else einfach die Logik umdrehen
            skip_if_block = !skip_if_block;
        }

        // Block-Ende: \endif
        else if (inside_if_block && line.find(R"(\endif)") != std::string::npos) {
            inside_if_block = false;
        }

        // Zeilenausgabe: nur wenn erlaubt
        else {
            if (!inside_if_block || !skip_if_block) {
                output << line << "\n";
            }
            // (ansonsten, Zeile wird einfach ignoriert)
        }
    }

    return output.str();
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
 *     -> \begin{lstlisting}[language=cpp]
 *         int main() { return 0; }
 *       \end{lstlisting}
 *
 * Parameter:
 * - input: Der vollständige Eingabetext mit potenziellen Codeblöcken.
 *
 * Rückgabe:
 * - Der Text, in dem alle Codeblock-Makros durch LaTeX-kompatible Listings ersetzt wurden.
 */
std::string simplify_codeblocks(const std::string& input) {
    
    std::string result = input;
    std::string code = "";
    std::string latex_block = "";
    std::string language = "";
    size_t pos = 0;
    size_t lang_end = 0;
    size_t brace_start = 0;
    size_t brace_end = 0; 


    while ((pos = result.find("#codeblock[", pos)) != std::string::npos) {

        language = extract_language(result, pos, lang_end);
       
        if (lang_end == std::string::npos) {

            pos += 1;  // ungültige Syntax -> weiter
            continue;
        }

    

        // Suchen nach der öffnenden geschweiften Klammer '{'
        brace_start = result.find('{', lang_end);
        if (brace_start == std::string::npos) {
            std::cerr << "+++ Fehler beim Finden der geöffneten Klammer [ { ] +++ " << "\n";
            pos += 1;
            continue;
        }

        // Extrahiere den Quellcode-Block
        brace_end = 0;
        code = extract_codeblock_body(result, brace_start, brace_end);

        if (code.empty()) {
            std::cerr << "+++ Fehler beim Code Block +++ " << "\n";
            pos += 1;  // Ungültiger oder unvollständiger Block
            continue;
        }

        // Latex-Befehl erzeugen
        latex_block =
            "\\begin{lstlisting}[language=" + language + "]\n" +
            code + "\n\\end{lstlisting}";

        // Ersetzen des ursprünglichen Makros durch das LaTeX-Listing
        result.replace(pos, brace_end - pos + 1, latex_block);

        // Weiter hinter dem eingefügten Block
        pos += latex_block.length();
    }

    return result;
}


