#pragma once

#include <string>
#include <vector>
#include <optional>

struct CliConfig {
    std::string input_file;
    std::string output_file = "output.tex";
    std::string format = "latex";
    std::string macro_file = "macros.json";
    std::vector<std::string> defines;
};

// Funktion zum Parsen der Kommandozeilenargumente mit Hilfe der cxxopts-Bibliothek
std::optional<CliConfig> parse_cli_args(int argc, char* argv[]);