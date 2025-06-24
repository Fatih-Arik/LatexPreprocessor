
#include "macro_utils.h"
#include "preprocessor.h"
#include "file_utils.h"


#include <iostream>


/**
 * Extrahiert die Argumente eines Makros mit runder Klammer-Syntax wie:
 *   - \frac(1, 2)
 *   - \sqrt(frac(4, 9))
 *
 * Die Funktion erkennt auch verschachtelte Makros korrekt (z. B. \frac(1, \sqrt(2))).
 *
 * Parameter:
 * - text: Gesamter Quelltext.
 * - start_pos: Position der ersten öffnenden Klammer `(`.
 * - end_pos: Wird gesetzt auf Position der schließenden Klammer `)` (bei Erfolg).
 *
 * Rückgabe:
 * - Liste der extrahierten Argumente (bei Fehler: leerer Vektor, end_pos = text.size()).
 */

std::vector<std::string> extract_math_args(const std::string& text, size_t start_pos, size_t& end_pos) {


    int brace_depth = 0;
    std::vector<std::string> result;
    std::string arg;

    for (size_t i = start_pos; i < text.size(); i++) {
        char c = text[i];


        if (c == '(') {
            brace_depth++;
            if (brace_depth > 1) {
                // gehört zu einem inneren Ausdruck (z. B. \frac(1, \sqrt(2)))
                arg += c;
            }
        }
        else if (c == ')') {
            brace_depth--;

            if (brace_depth == 0) {
                // Oberste Klammer geschlossen -> Ausdruck ist zu Ende
                result.push_back(arg);
                end_pos = i; // setzen end_pos = i für die simplify_macros Funktion
                return result;
            }
            else {
                arg += c;
            }
        }
        else if (c == ',' && brace_depth == 1) {
            // Trennzeichen auf oberster Ebene -> Argument abschließen
            result.push_back(arg);
            arg.clear();
        }
        else {
            arg += c;
        }
    }

    // Falls keine schließende Klammer gefunden -> Fehlerbehandlung
    end_pos = text.size();
    return {};
}

/**
 * Ersetzt Platzhalter wie "__0__", "__1__" usw. im Formatstring durch die übergebenen Argumente.
 *
 * Nützlich zum dynamischen Erzeugen von LaTeX-Ausdrücken wie \frac{...}{...}.
 *
 * Parameter:
 *     replacement – Der Formatstring mit nummerierten Platzhaltern.
 *     args   – Die Argumente, die eingesetzt werden sollen.
 *
 * Rückgabe:
 *     Der fertige String, in dem alle Platzhalter ersetzt wurden.
 */
std::string apply_format(const std::string& replacement, const std::vector<std::string>& args) {
    std::string result = replacement;
    for (size_t i = 0; i < args.size(); i++) {
        std::string placeholder = "__" + std::to_string(i) + "__";
        size_t pos;
        // Ersetze alle Vorkommen von "__i__" 
        while ((pos = result.find(placeholder)) != std::string::npos) {

            result.replace(pos, placeholder.length(), args[i]);
        }
    }

    return result;
}




/**
 * Vereinfacht rekursiv ein bestimmtes Makro im Text anhand der übergebenen Spezifikation.
 *
 * Beispiel:
 *     Eingabe:  "\frac(1, 2)"
 *     Spezifikation: { "frac", 2, "\\frac{__0__}{__1__}" }
 *     Ausgabe: "\\frac{1}{2}"
 *
 * Parameter:
 *     text – Der gesamte Quelltext, in dem das Makro ersetzt werden soll.
 *     spec – Struktur mit Makroname, erwarteter Argumentanzahl und LaTeX-Formatstring.
 *
 * Rückgabe:
 *     Der überarbeitete Text mit allen vorkommenden Makros ersetzt.
 */
std::string simplify_macro_spec(const std::string& text, const macro_spec& spec) {
    std::string result = text;
    size_t pos = 0;
    size_t end_pos = 0;

    while ((pos = result.find(spec.name, pos)) != std::string::npos) {
        // Position der ersten öffnenden Klammer (direkt nach dem Makronamen)
        size_t open_paren = pos + spec.name.length();

        // Versuche, die Argumente auszulesen
        std::vector<std::string> args = extract_math_args(result, open_paren, end_pos);


        // Fehler: falsche Anzahl an Argumenten
        if (args.size() != spec.arg_count) {
            std::cerr << "+++ Fehler bei '" << spec.name << "': erwartet " << spec.arg_count
                << " Argument(e), aber " << args.size() << " gefunden. +++\n";
            pos += 1;
            continue;
        }

        // Rekursive Vereinfachung der Argumente
        for (std::string& arg : args) {
            arg = simplify_macro_spec(arg, spec);
        }

        // Ersetze durch LaTeX-Befehl
        std::string replacement = apply_format(spec.replacement, args);
        result.replace(pos, end_pos - pos + 1, replacement);

        // Fahre an der neuen Position fort
        pos += replacement.length();
    }

    return result;
}




/**
 * Wandelt Ausdrücke wie #math(...) oder #blockmath(...) in die entsprechende LaTeX-Umgebung um.
 *
 * Beispiel:
 *   #math(\frac(1, 2))      -> \(\frac(1, 2)\)
 *   #blockmath(\frac(1, 2)) -> \[\frac(1, 2)\]
 *
 * Diese Funktion erkennt das passende Makro (#math oder #blockmath), extrahiert den Ausdruck
 * und umschließt ihn mit LaTeX-Inline- bzw. Block-Mathe-Klammern.
 *
 * Parameter:
 *   - text: Der Quelltext, der nach Math-Makros durchsucht werden soll.
 *   - is_block: true für Block-Mathe (\[...\]), false für Inline-Mathe (\(...\)).
 *
 * Rückgabe:
 *   - Der überarbeitete Text mit ersetzten Mathe-Makros.
 */
std::string simplify_math_wrapper(const std::string& text, bool is_block) {
    std::string result = text;
    size_t pos = 0;

    // Je nach Modus richtigen Makro-Start und Wrapper wählen
    std::string macro_prefix = is_block ? "#blockmath" : "#math";
    std::string wrapper_start = is_block ? "\\[" : "\\(";
    std::string wrapper_end = is_block ? "\\]" : "\\)";

    while ((pos = result.find(macro_prefix, pos)) != std::string::npos) {
        size_t open_paren = pos + macro_prefix.length();
        size_t end_pos;

        // Extrahiere das Argument (also der mathematische Ausdruck)
        std::vector<std::string> args = extract_math_args(result, open_paren, end_pos);
        if (args.size() != 1) {
            pos += macro_prefix.length();  // Fehlerhafter Ausdruck, z. B. fehlende schließende Klammer
            continue;
        }

        std::string latex_expr = wrapper_start + args[0] + wrapper_end;

        // Ersetze gesamten Makroausdruck
        result.replace(pos, end_pos - pos + 1, latex_expr);
        pos += latex_expr.length();
    }

    return result;
}

/**
 * Wandelt alle #math(...)-Makros in LaTeX-Inline-Mathe (\(...\)) um.
 *
 * Beispiel:
 *   #math(\frac(1, 2)) -> \( \frac{1}{2} \)
 *
 * Parameter:
 *   - text: Der Text mit #math-Ausdrücken.
 *
 * Rückgabe:
 *   - Der Text mit umgewandelten Inline-Mathe-Ausdrücken.
 */
std::string simplify_inline_math(const std::string& text) {
    return simplify_math_wrapper(text, false);
}

/**
 * Wandelt alle #blockmath(...)-Makros in LaTeX-Block-Mathe (\[...\]) um.
 *
 * Beispiel:
 *   #blockmath(\frac(1, 2)) -> \[ \frac{1}{2} \]
 *
 * Parameter:
 *   - text: Der Text mit #blockmath-Ausdrücken.
 *
 * Rückgabe:
 *   - Der Text mit umgewandelten Block-Mathe-Ausdrücken.
 */
std::string simplify_block_math(const std::string& text) {
    return simplify_math_wrapper(text, true);
}



