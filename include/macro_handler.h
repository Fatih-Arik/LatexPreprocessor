#pragma once

#include "source_line.h"
#include "error_collector.h"

#include <string>
#include <unordered_map>


// Makrotyp zur Unterscheidung von Format- und Logik-Makros
enum class macro_type {
    Format,       // z. B. \frac, \sqrt 
    Conditional,  // \ifdef{...}...\else...\endif
    Include,      // \include{"datei.tex"}
    Define        // \define{KEY}, \define{KEY}{VAL}
};

/**
 * Struktur zur Beschreibung eines dynamischen Makros.
 * Diese Makros werden aus einer JSON-Datei geladen und können beliebige Regeln enthalten.
 */
struct dynamic_macro {
    macro_type type = macro_type::Format; // Typ des Makros (macro_type) | default = Format
    std::string name;                     // Makroname, z. B. \frac 
    size_t arg_count = 0;                 // Anzahl der Argumente für Format-Makros
    std::string replacement;              // Ersatztext (z. B. \frac{__0__}{__1__})
};

/**
 * Lädt alle dynamischen Makros aus einer JSON-Konfigurationsdatei.
 */
std::unordered_map<std::string, dynamic_macro> load_all_macros(const std::string& path, PreprocReport& report);

/**
 * Wendet alle erkannten Makros (Format und Logik) auf den Eingabetext an.
 */
std::vector<SourceLine> apply_all_macros(const std::vector<SourceLine>& content,
    const std::unordered_map<std::string, dynamic_macro>& macros,
    const std::unordered_map<std::string, std::string>& defines,
    PreprocReport& report
);


