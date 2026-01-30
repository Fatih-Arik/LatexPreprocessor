// ------------------------------
// LaTeX Preprocessor
// Autor: [Fatih Arik]
// Beschreibung: Präprozessor für vereinfachte LaTeX-artige Makros
// Makro Konfiguration über config/dynamic_macro.json
// ------------------------------


#include "preprocessor.h"
#include "file_utils.h"
#include "cli_utils.h"
#include "macro_handler.h"
#include "error_collector.h"
#include "source_line.h"


#include <iostream>
#include <unordered_map>
#include <string>


//#define DEBUG_MODE             



void log_step(const std::string& title, const std::string& content) {
    #ifdef DEBUG_MODE
    std::cout << "\n### " << title << " ###\n" << content << "\n";
    #endif
}



/**
 * Führt den LaTeX-Präprozessor mit den übergebenen Kommandozeilenargumenten aus.
 *
 * Ablauf:
 *   1. Parsen der CLI-Argumente
 *   2. Einlesen der Eingabedatei
 *   3. Laden der Makrodefinitionen (JSON)
 *   4. Extraktion von \define-Makros
 *   5. Anwendung aller dynamischen Makros
 *   6. Ausgabe in die Zieldatei
 *
 * Rückgabewerte:
 *   0  – Verarbeitung erfolgreich
 *   1  – Fehler
 */
int run_preprocessor(int argc, char* argv[]) {

    auto configOpt = parse_cli_args(argc, argv);
    if (!configOpt) {
        return -1;
    }

    CliConfig config = *configOpt;

    std::cout << "Eingabedatei: " << config.input_file  << "\n"
              << "Ausgabedatei: " << config.output_file << "\n"
              << "Makro-Datei: "  << config.macro_file  << "\n";

    std::vector<SourceLine> content = read_file_lines(config.input_file);
    //std::string content = read_file(config.input_file);
    if (content.empty()) {
        return -1;  
    }

    PreprocReport report;
   

    // Makros aus JSON laden
    std::unordered_map<std::string, dynamic_macro> all_macros = load_all_macros(config.macro_file, report);


    std::unordered_set<std::string> include_stack;
    content = process_include(content, report, include_stack);


    // \define-Makros aus dem Text extrahieren
    std::unordered_map<std::string, std::string> define_macros = extract_defines(content, report);

     
    // Alle Makros anwenden
    content = apply_all_macros(content, all_macros, define_macros, report);

    // Fehlerbericht auswerten
    if (report.has_errors()) {
        std::cerr << "Es sind Fehler aufgetreten:\n";
        for (auto& e : report.errors) {
            std::cerr << "[Fehler] in " << e.file << " - ";
            if (e.line > 0) {
                std::cerr << "Zeile " << e.line;
            }
            std::cerr << ": " << e.message << "\n";
        }
        return -1; // Verarbeitung abbrechen

    }
    save_to_file(config.output_file, content);

    return 0;

}
int main(int argc, char* argv[]) {
    
    std::cout << "LaTeX Preprocessor...\n";

    return run_preprocessor(argc, argv);
}
