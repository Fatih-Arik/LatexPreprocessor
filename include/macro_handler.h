#pragma once


#include <string>
#include <unordered_map>


// Makrotyp zur Unterscheidung von Format- und Logik-Makros
enum class macro_type {
    Format,       // z. B. \frac, \sqrt 
    Codeblock,    // z. B. #codeblock[lang]{...}
    Conditional,  // \ifdef{...}...\else...\endif
    Include,      // \include{"datei.tex"}
    Define        // \define{KEY}, \define{KEY}{VAL}
};

/**
 * Struktur zur Beschreibung eines dynamischen Makros.
 * Diese Makros werden aus einer JSON-Datei geladen und können beliebige Regeln enthalten.
 */
struct dynamic_macro {
    macro_type type;          // Typ des Makros (macro_type)
    std::string name;         // Makroname, z. B. \frac oder #codeblock
    size_t arg_count = 0;     // Anzahl der Argumente für Format-Makros
    std::string replacement;       // Ersatztext (z. B. \frac{__0__}{__1__})
};

/**
 * Lädt alle dynamischen Makros aus einer JSON-Konfigurationsdatei.
 */
std::unordered_map<std::string, dynamic_macro> load_all_macros(const std::string& path);

/**
 * Wendet alle erkannten Makros (Format und Logik) auf den Eingabetext an.
 */
std::string apply_all_macros(const std::string& input_text,
    const std::unordered_map<std::string, dynamic_macro>& macros,
    const std::unordered_map<std::string, std::string>& defines);


