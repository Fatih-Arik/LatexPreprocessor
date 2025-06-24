#pragma once

#include "macro_utils.h"
#include "json.hpp"

#include <string>
#include <unordered_map>
#include <vector>

// Liest den gesamten Inhalt einer Datei in einen String.
std::string read_file(const std::string& filename);


// Speichert den Inhalt in eine Datei.
void save_to_file(const std::string& filename, const std::string& content);

// Liest den Inhalt einer JSON-Datei und gibt das JSON-Objekt zurück.
nlohmann::json read_json_config(const std::string& filename);

//Liest den Inhalt von macros.json und gibt den Inhalt als unordered_map zurück
std::unordered_map<std::string, macro_spec> load_macros_from_file(const std::string& filename);