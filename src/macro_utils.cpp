
#include "macro_utils.h"
#include "preprocessor.h"

#include <iostream>
#include <sstream>

/**
 * Extrahiert die Argumente eines Makros mit runder Klammer-Syntax wie:
 *   - \frac{1, 2}
 *   - \sqrt{frac{4, 9}}
 *
 * Die Funktion erkennt auch verschachtelte Makros korrekt (z. B. \frac{1, \sqrt{2}}).
 *
 * Parameter:
 * - text: Gesamter Quelltext.
 * - start_pos: Position der ersten öffnenden Klammer `{`.
 * - end_pos: Wird gesetzt auf Position der schließenden Klammer `}` (bei Erfolg).
 *
 * Rückgabe:
 * - Liste der extrahierten Argumente (bei Fehler: leerer Vektor, end_pos = text.size()).
 */

std::vector<std::string> extract_math_args(const std::string& text, size_t start_pos, size_t& end_pos) {

    int brace_depth = 0;
    std::vector<std::string> result;
    std::string arg;
	size_t i = start_pos;

    for (; i < text.size(); i++) {

        char c = text[i];

        if (c == '{') {
            brace_depth++;
            if (brace_depth > 1) {
                // gehört zu einem inneren Ausdruck (z. B. \frac{1, \sqrt{2})
                arg += c;
            }
        }
        else if (c == '}') {
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
    end_pos = start_pos;
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
 * Vereinfacht rekursiv ein bestimmtes Formatmakro im Text anhand
 * der übergebenen Makrospezifikation.
 *
 * Die Funktion durchsucht jede Quellzeile nach Vorkommen des
 * angegebenen Makros (z. B. \frac{...}) und ersetzt diese durch
 * das zugehörige Formatmuster. Enthalten die Makroargumente
 * selbst weitere Formatmakros, erfolgt die Verarbeitung rekursiv.
 *
 * Beispiel:
 *     Eingabe:  "\frac{1, 2}"
 *     Spezifikation: { "frac", 2, "\\frac{__0__}{__1__}" }
 *     Ausgabe: "\\frac{1}{2}"
 *
 * Fehlerhafte Makroaufrufe (z. B. falsche Argumentanzahl) werden
 * nicht ersetzt, sondern als Fehler im PreprocReport vermerkt.
 * Die weitere Verarbeitung wird dabei fortgesetzt.
 *
 * Parameter:
 *     text   – Eingabetext als Liste von SourceLine-Strukturen
 *              (enthält Zeilentext, Dateiname und Zeilennummer).
 *     spec   – Makrospezifikation (Name, Argumentanzahl, Ersetzungsregel).
 *     report – Fehlerbericht zur Sammlung von Syntax- und Verarbeitungsfehlern.
 *
 * Rückgabe:
 *     Neuer Vektor von SourceLine-Objekten mit ersetzten Makros.
 */std::vector<SourceLine> simplify_macro_spec(
	 const std::vector<SourceLine>& text,
	 const macro_spec& spec,
	 PreprocReport& report)
 {
	 size_t macro_pos = 0;   // Aktuelle Suchposition innerhalb der Zeile
	 size_t end_pos = 0;   // Endposition des vollständigen Makroausdrucks


	 std::vector<SourceLine> result = text;

	 for (SourceLine& sl : result) {

		 macro_pos = 0;

		 // Suche nach Vorkommen des Makros (z. B. "\frac{...}")
		 while ((macro_pos = sl.line.find(spec.name + '{', macro_pos)) != std::string::npos) {



			 // Argumente aus dem Makro extrahieren
			 end_pos = 0;
			 std::vector<std::string> args =
				 extract_math_args(
					 sl.line,
					 macro_pos + spec.name.size(),
					 end_pos
				 );

			 // Nach erstem Argument
			 if (end_pos + 1 < sl.line.size() && sl.line[end_pos + 1] == '{') {
				 // vermutlich echtes LaTeX \frac{a}{b}
				 macro_pos += spec.name.size();
				 continue;
			 }

			 // Fehlerfall: falsche Anzahl an Argumenten
			 if (args.size() != spec.arg_count) {
				 report.errors.push_back({
					 sl.file,
					 "Fehler bei '" + spec.name +
					 "': erwartet " + std::to_string(spec.arg_count) +
					 " Argument(e), aber " + std::to_string(args.size()) +
					 " gefunden.",
					 sl.line_nr
					 });

				 // Weitersuchen hinter dem Makronamen, um Endlosschleifen zu vermeiden
				 macro_pos += spec.name.size();
				 continue;
			 }

			 // Rekursive Verarbeitung der Argumente (falls diese selbst Makros enthalten)
			 for (std::string& arg : args) {
				 std::vector<SourceLine> tmp;
				 tmp.push_back({
					 arg,
					 sl.file,
					 sl.line_nr
					 });

				 tmp = simplify_macro_spec(tmp, spec, report);
				 arg = tmp[0].line;
			 }

			 // Ersetzung des Makroaufrufs durch den formatierten LaTeX-Ausdruck
			 std::string replacement = apply_format(spec.replacement, args);
			 sl.line.replace(
				 macro_pos,
				 end_pos - macro_pos + 1,
				 replacement
			 );

			 // Suchposition hinter das ersetzte Makro verschieben
			 macro_pos += replacement.size();
		 }
	 }
	return result;
 }

