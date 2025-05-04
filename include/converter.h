#pragma once
#include <string>

// Erstellt eine PDF-Datei aus einer LaTeX-Datei mittels pdflatex.
int generate_pdf(const std::string& output_folder, const std::string& output_file, const std::string& pdflatex_path);