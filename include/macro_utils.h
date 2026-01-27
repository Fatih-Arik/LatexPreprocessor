#pragma once
#include <string>
#include <vector>

#include "source_line.h"
#include "error_collector.h"


struct macro_spec {
    std::string name;          // z.B. "\frac", "\sqrt"
    size_t arg_count;          // Anzahl erwarteter Argumente
    std::string replacement;   // Format-String mit Platzhaltern, z. B. "\\frac{__0__}{__1__}"
};



// Extrahiert die Argumente eines Makros mit geschweifter Klammer - Syntax
std::vector<std::string> extract_math_args(const std::string& text, size_t start_pos, size_t& end_pos);


// Vereinfacht rekursiv ein bestimmtes Makro im Text anhand der übergebenen Spezifikation.
std::vector<SourceLine> simplify_macro_spec(const std::vector<SourceLine>& text, const macro_spec& spec, PreprocReport& report);


// Ersetzt Platzhalter im Formatstring (z. B. "__0__") durch Argumente.
std::string apply_format(const std::string& replacement, const std::vector<std::string>& args);

