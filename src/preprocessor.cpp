#include "preprocessor.h"
#include "file_utils.h"
#include "macro_utils.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <vector>


/**
 * Ersetzt rekursiv alle \include{...}-Anweisungen im gegebenen Text durch den Inhalt
 * der referenzierten Dateien.
 *
 * Die Funktion liest den Eingabetext zeilenweise und prüft, ob eine Zeile mit \include beginnt.
 * In diesem Fall wird der in geschweiften Klammern angegebene Dateiname extrahiert,
 * der zugehörige Inhalt mit read_file(...) geladen und anschließend erneut durch
 * process_include(...) verarbeitet, um verschachtelte Includes zu unterstützen.
 *
 * Nicht erkannte oder fehlerhaft formatierte Include-Zeilen bleiben unverändert erhalten.
 *
 * Parameter:
 *     content – Der gesamte Eingabetext, in dem \include{...}-Anweisungen verarbeitet werden sollen.
 *
 * Rückgabe:
 *     Ein neuer Text, in dem alle \include-Anweisungen durch den vollständigen, rekursiv
 *     aufgelösten Dateiinhalt ersetzt wurden.
 */
std::string process_include(const std::string& content) {

    std::ostringstream result;
    std::istringstream input(content);
    std::string line;
    std::string include_file;
    std::string included_content;
    std::size_t open_bracket;
    std::size_t close_bracket;

    while (std::getline(input, line)) {

        // Prüft, ob die Zeile mit dem Include-Makro beginnt
        if (line.starts_with("\\include")) {

            // Bestimme die Positionen der geschweiften Klammern
            open_bracket = line.find("{") + 1;
            close_bracket = line.find("}", open_bracket);

            // Fehlerbehandlung: Ungültige Klammerpositionen -> Zeile unverändert übernehmen
            if (open_bracket == std::string::npos || close_bracket == std::string::npos) {
                std::cerr << "+++ Fehler bei \\include +++ " << "\n";
                result << line << "\n";
                continue;
            }

            // Extrahiere Dateinamen zwischen den Klammern
            include_file = line.substr(open_bracket, close_bracket - open_bracket);

            // Lade Dateiinhalte und füge sie in den Text ein
            included_content = read_file(include_file);
            //TODO Rekursiver aufruf Include
            included_content = process_include(included_content);
            result << included_content << "\n";
        }
        else {
            result << line << "\n";
        }
    }
    
    return result.str();
}



/**
 * Extrahiert alle \define-Makros aus dem LaTeX-Quelltext.
 *
 * Beispiele:
 *   \define{AUTHOR}{Max}   → "AUTHOR" → "Max"
 *   \define{DEBUG}         → "DEBUG"  → ""
 * 
 * Parameter:
 *      content – Der vollständige Eingabetext, in dem nach Makros gesucht wird.
 * Rückgabe:
 *      Eine HashMap (unordered_map), die alle gefundenen Makros enthält.
 */

std::unordered_map<std::string, std::string> extract_defines(const std::string& content) {
    
    std::unordered_map<std::string, std::string> macros;
    std::istringstream input(content);

    std::string key;
    std::string value;
    std::string line;
    size_t key_open_bracket;
    size_t key_close_bracket;
    size_t value_open_bracket;
    size_t value_close_bracket;

        
    while (std::getline(input, line)) {
        
        if (!line.starts_with("\\define")) {
            continue;
        }
        // vom Define-Makro den Key extrahieren
        key_open_bracket = line.find("{");
        key_close_bracket = line.find("}", key_open_bracket);

        // Fehlerbehandlung: Ungültige Klammerpositionen
        if (key_open_bracket == std::string::npos || key_close_bracket == std::string::npos) {
            std::cerr << "+++ Syntaxfehler in \\define-Zeile (kein Key): " << line << "\n";
            continue;
        }
        // Der Makro Bezeichner soll als Schlüssel des HashMaps gespeichert werden
        // {wert}
        key = line.substr(key_open_bracket + 1, key_close_bracket - key_open_bracket - 1);

        //ggf. enthält der Define Makro einen Value 
        value_open_bracket = line.find("{", key_close_bracket);
        value_close_bracket = line.find("}", value_open_bracket);


        // Wenn der Define-Makro einen Value besitzt
        if (value_open_bracket != std::string::npos && value_close_bracket != std::string::npos) {
            std::string value = line.substr(value_open_bracket + 1, value_close_bracket - value_open_bracket - 1);
            macros[key] = value;
        }
        else if (value_open_bracket == std::string::npos && value_close_bracket == std::string::npos) {
            macros[key] = "";  // Kein Wert
        }
        else {
            std::cerr << "+++ Syntaxfehler in \\define-Zeile (unvollständiger Value): " << line << "\n";
            continue;
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

std::string replace_text_macros(
    const std::string& text,
    const std::unordered_map<std::string, std::string>& macros)
{
    std::string result = text;

    for (const auto& [key, value] : macros) {
        // \bKEY\b   -> exakte Wortübereinstimmung (KEY als ganzes Wort)
        std::string pattern_str = "\\b" + key + "\\b";
        std::regex pattern(pattern_str);

        result = std::regex_replace(result, pattern, value);
    }
    
    return result;
}

/**
 * Entfernt alle `\define`-Makros aus dem Text.
 *
 * Dabei wird jede Zeile, die mit `\define` beginnt, vollständig entfernt.
 *
 * Parameter:
 *     text – Der gesamte LaTeX-Eingabetext.
 *
 * Rückgabe:
 *     Der bereinigte Text ohne `\define`-Makros.
 */
std::string remove_defines(const std::string& text) {
    std::istringstream input_stream(text);
    std::ostringstream output_stream;
    std::string line;

    while (std::getline(input_stream, line)) {
        // Zeilen, die mit "\define" beginnen, überspringen
        if (line.starts_with("\\define")) {
            continue;
        }

        output_stream << line << '\n';
    }

    return output_stream.str();
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


