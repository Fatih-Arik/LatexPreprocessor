#include "cli_utils.h"
#include "cxxopts.hpp"

#include <iostream>


#ifndef PROJECT_DIR
#define PROJECT_DIR "."
#endif

//In der CMakeLists.txt wird eine eine globale Konstante zur Kompilierzeit definiert: PROJECT_DIR
//Wird im C++ - Code als Pfad zur Projektwurzel verwendet
//add_compile_definitions(PROJECT_DIR = "${CMAKE_SOURCE_DIR}")
std::filesystem::path get_default_macro_path() {
    return std::filesystem::path(PROJECT_DIR) / "config" / "dynamic_macro.json";
}

std::filesystem::path get_default_output_path() {
    return std::filesystem::path(PROJECT_DIR) / "output" / "test_output.tex";
}


// Funktion zum Parsen der Kommandozeilenargumente mit Hilfe der cxxopts-Bibliothek
std::optional<CliConfig> parse_cli_args(int argc, char* argv[]){
    try {
        // Initialisierung die Optionen mit Programmname und Beschreibung
        cxxopts::Options options("LatexPreprocessor", "LaTeX-Präprozessor: Konvertiert vereinfachte Syntax in LaTeX");

        // Definiere die erwarteten Kommandozeilenoptionen
        options.add_options()
            ("o,output", "Ausgabedatei", cxxopts::value<std::string>()->default_value(get_default_output_path().generic_string()))
            ("f,format", "Ausgabeformat (latex, markdown, html)", cxxopts::value<std::string>()->default_value("latex"))
            ("m,macros", "Pfad zur Makrodefinition (JSON)", cxxopts::value<std::string>()->default_value(get_default_macro_path().generic_string()))
            ("input", "Eingabedatei (Pflichtparameter)", cxxopts::value<std::string>())
            ("h,help", "Hilfe anzeigen");

        // Definiere, dass der Positionsparameter (ohne Flag) als Eingabedatei interpretiert wird
        options.parse_positional({ "input" });
        options.positional_help("input.tex");

        // Parsen der Argumente
        auto result = options.parse(argc, argv);

        // Wenn Hilfe angezeigt werden soll oder Eingabedatei fehlt
        if (result.count("help") || !result.count("input")) {
            std::cout << options.help() << std::endl;
            return std::nullopt;
        }

        // Konfiguration auslesen und befüllen
        CliConfig config;
        config.input_file = result["input"].as<std::string>();
        config.output_file = result["output"].as<std::string>();
        config.format = result["format"].as<std::string>();
        config.macro_file = result["macros"].as<std::string>();


        return config; // Erfolgreich geparste Konfiguration zurückgeben

    }
    catch (const std::exception& e) {
        std::cerr << "Fehler beim Parsen der Argumente: " << e.what() << "\n";
        return std::nullopt; // Bei Fehler keine Konfiguration zurückgeben
    }
}
