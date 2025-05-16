// ------------------------------
// LaTeX Preprocessor
// Autor: [Fatih Arik]
// Beschreibung: Präprozessor für vereinfachte LaTeX-artige Makros
// Konfiguration über config/config.json
// ------------------------------


#include "../include/preprocessor.h"
#include "../include/file_utils.h"
#include "../include/converter.h"
#include "../include/macro_utils.h"


#include <iostream>
#include <unordered_map>



//#define TEST_MODE
//#define DEBUG_MODE

#ifdef TEST_MODE
#include "../tests/test_runner.h"
#endif


void log_step(const std::string& title, const std::string& content) {
    #ifdef DEBUG_MODE
    std::cout << "\n### " << title << " ###\n" << content << "\n";
    #endif
}

int run_preprocessor(const nlohmann::json& config) {

    std::string input_file = config.value("input_file", "latex_docs/test_input.tex");
    std::string output_file = config.value("output_file", "output/test_output.tex");
    std::string output_folder = config.value("output_folder", "output/");
    std::string pdflatex_path = config.value("pdflatex_path", "C:/Users/fatih/AppData/Local/Programs/MiKTeX/miktex/bin/x64/pdflatex.exe");
    std::string output_format = config.value("output_format", "latex"); 
    // ##TODO## bool keep_intermediate_files = config.value("keep_intermediate_files", false); 


    std::string content = read_file(input_file);
    if (content.empty()) {
        //    std::cerr << "Fehler: Datei konnte nicht gelesen werden: " << input_file << "\n";
        return -1;
    }

    log_step("Input File", content);


    // Include-Verarbeitung
    content = process_include(content);
    log_step("Include processing", content);


    std::unordered_map<std::string, std::string> macros = extract_defines(content);
    content = replace_text_macros(content, macros);
    log_step("Text replaced", content);


    content = remove_defines(content);
    log_step("Defines removed", content);

    content = simplify_all_macros(content);
    log_step("Macros simplified", content);

    save_to_file(output_file, content);


    if (generate_pdf(output_folder, output_file, pdflatex_path) == 0) {
        std::cout << "### PDF erfolgreich generiert! ###" << "\n";

        //TODO
        //if (!keep_intermediate_files) {
        //    //cleanup_intermediate_files(output_folder, output_file);
        //    std::cout << "Temporäre Dateien entfernt.\n";
        //}
    }
    else {
        std::cerr << "+++ Fehler beim Generieren der PDF! +++" <<  "\n";
    }
    return 0;

}
int main() {
    #ifdef TEST_MODE
    run_all_tests();
    return 0;
    #else
    
    std::cout << "### Latex Preprocessor... ###\n";
    nlohmann::json config = read_json_config("config/config.json");
    std::cout << "### Konfiguration geladen. ###" << "\n";

    run_preprocessor(config);
    return 0;
    #endif
}
