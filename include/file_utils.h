#pragma once

#include "source_line.h"
#include "json.hpp"


#include <string>
#include <unordered_map>
#include <vector>

// Liest den gesamten Inhalt einer Datei in einen String.
std::vector<SourceLine> read_file_lines(const std::string& filename);


// Speichert den Inhalt in eine Datei.
void save_to_file(const std::string& filename, const std::vector<SourceLine>& content);

// Liest den Inhalt einer JSON-Datei und gibt das JSON-Objekt zurück.
nlohmann::json read_json_config(const std::string& filename);

