// ------------------------------
// LaTeX Preprocessor
// Autor: [Fatih Arik]
// Beschreibung: Präprozessor für vereinfachte LaTeX-artige Makros
// Makro Konfiguration über config/macros.json
// ------------------------------


#include "../include/preprocessor.h"
#include "../include/file_utils.h"
#include "../include/converter.h"
#include "../include/macro_utils.h"
#include "../include/cli_utils.h"


#include <iostream>
#include <unordered_map>


//#define TEST_MODE
#define DEBUG_MODE

#ifdef TEST_MODE
#include "../tests/test_runner.h"
#endif


void log_step(const std::string& title, const std::string& content) {
    #ifdef DEBUG_MODE
    std::cout << "\n### " << title << " ###\n" << content << "\n";
    #endif
}

int run_preprocessor(int argc, char* argv[]) {


    auto configOpt = parse_cli_args(argc, argv);
    if (!configOpt) return 1;

    CliConfig config = *configOpt;

    std::cout << "Eingabedatei: " << config.input_file << "\n";
    std::cout << "Ausgabedatei: " << config.output_file << "\n";
    std::cout << "Format: " << config.format << "\n";
    std::cout << "Makro-Datei: " << config.macro_file << "\n";

    //std::string input_file = config.value("input_file", "latex_docs/test_input.tex");
    //std::string output_file = config.value("output_file", "output/test_output.tex");
    //std::string output_folder = config.value("output_folder", "output/");
    //std::string pdflatex_path = config.value("pdflatex_path", "C:/Users/fatih/AppData/Local/Programs/MiKTeX/miktex/bin/x64/pdflatex.exe");
    //std::string output_format = config.value("output_format", "latex"); 
    //// ##TODO## bool keep_intermediate_files = config.value("keep_intermediate_files", false); 


    std::string content = read_file(config.input_file);
    if (content.empty()) {
        //    std::cerr << "Fehler: Datei konnte nicht gelesen werden: " << input_file << "\n";
        return -1;
    }

    log_step("Input File", content);


    // Include-Verarbeitung
    content = process_include(content);
    log_step("Include processing", content);

    // Extrahierung der Einfachen-Makros
    std::unordered_map<std::string, std::string> macros = extract_defines(content);
    content = replace_text_macros(content, macros);
    log_step("Text replaced", content);

    // Enterfernen der Defines
    content = remove_defines(content);
    log_step("Defines removed", content);


    // Auführung der Ersetzung
    content = simplify_all_macros(content, config.macro_file);
    log_step("Macros simplified", content);

    save_to_file(config.output_file, content);


    //if (generate_pdf(output_folder, output_file, pdflatex_path) == 0) {
    //    std::cout << "### PDF erfolgreich generiert! ###" << "\n";

    //    //TODO
    //    //if (!keep_intermediate_files) {
    //    //    //cleanup_intermediate_files(output_folder, output_file);
    //    //    std::cout << "Temporäre Dateien entfernt.\n";
    //    //}
    //}
    //else {
    //    std::cerr << "+++ Fehler beim Generieren der PDF! +++" <<  "\n";
    //}
    return 0;

}
int main(int argc, char* argv[]) {
    #ifdef TEST_MODE
    run_all_tests();
    return 0;
    #else
    
    std::cout << "### Latex Preprocessor... ###\n";

    run_preprocessor(argc, argv);
    return 0;
    #endif
}
