#pragma once
#include <string>
#include <vector>



struct MacroSpec {
    std::string name;          // z.B. "frac", "sqrt"
    size_t arg_count;          // Anzahl erwarteter Argumente
    std::string format;        // Format-String mit Platzhaltern, z. B. "\\frac{__0__}{__1__}"
};



// Extrahiert die Argumente eines Makros mit runder Klammer - Syntax
std::vector<std::string> extract_math_args(const std::string& text, size_t start_pos, size_t& end_pos);


// Vereinfacht rekursiv ein bestimmtes Makro im Text anhand der übergebenen Spezifikation.
std::string simplify_macro_spec(const std::string& text, const MacroSpec& spec);


// Ersetzt Platzhalter im Formatstring (z. B. "__0__") durch Argumente.
std::string apply_format(const std::string& format, const std::vector<std::string>& args);


// Wandelt Ausdrücke wie #math(...) oder #blockmath(...) in die entsprechende LaTeX-Umgebung um.
std::string simplify_math_wrapper(const std::string& text, bool is_block);


// Wandelt alle #math(...)-Makros in LaTeX-Inline-Mathe (\(...\)) um.
std::string simplify_inline_math(const std::string& text);


// Wandelt alle #blockmath(...)-Makros in LaTeX-Block-Mathe (\[...\]) um.
std::string simplify_block_math(const std::string& text);