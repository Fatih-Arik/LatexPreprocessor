#include "../include/converter.h"
#include <iostream>
#include <cstdlib>



int generate_pdf(const std::string& output_folder, const std::string& output_file, const std::string& pdflatex_path) {
    // -interaction = nonstopmode → Verhindert, dass pdflatex auf Benutzereingaben wartet.
    // -halt-on-error → Stoppt, wenn Fehler auftreten.
    // +output_file → Deine.tex - Datei wird verarbeitet.

    //std::string command = "\"C:/Users/fatih/AppData/Local/Programs/MiKTeX/miktex/bin/x64/pdflatex.exe\" -output-directory=" + output_folder + "-interaction=nonstopmode -halt-on-error " + output_file;


    if (pdflatex_path.empty() || output_folder.empty() || output_file.empty()) {
        std::cerr << "❌ Fehler: Ungültige Eingabeparameter für generate_pdf()\n";
        return -1;
    }

    std::string command = "\"" + pdflatex_path + "\"" + " -output-directory=" + output_folder + " -interaction=nonstopmode -halt-on-error " + output_file;
    std::cout << "Command: " << command << "\n";
    return system(command.c_str());
}
