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


#include <iostream>
#include <unordered_map>
#include <string>


//#define DEBUG_MODE



void log_step(const std::string& title, const std::string& content) {
    #ifdef DEBUG_MODE
    std::cout << "\n### " << title << " ###\n" << content << "\n";
    #endif
}

int run_preprocessor(int argc, char* argv[]) {


    auto configOpt = parse_cli_args(argc, argv);
    if (!configOpt) {
        return 1;
    }

    CliConfig config = *configOpt;

    std::cout << "Eingabedatei: " << config.input_file << "\n";
    std::cout << "Ausgabedatei: " << config.output_file << "\n";
    std::cout << "Format: " << config.format << "\n";
    std::cout << "Makro-Datei: " << config.macro_file << "\n";


    std::string content = read_file(config.input_file);
    if (content.empty()) {
        //    std::cerr << "Fehler: Datei konnte nicht gelesen werden: " << input_file << "\n";
        return -1;
    }

    std::unordered_map<std::string, dynamic_macro> all_macros = load_all_macros(config.macro_file);
    std::unordered_map<std::string, std::string> define_macros = extract_defines(content);
    content = apply_all_macros(content, all_macros, define_macros);

    save_to_file(config.output_file, content);


    return 0;

}
int main(int argc, char* argv[]) {
    
    std::cout << "### Latex Preprocessor... ###\n";

    run_preprocessor(argc, argv);
    return 0;
}
