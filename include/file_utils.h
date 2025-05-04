#pragma once

#include "../include/json.hpp"
#include <string>


// Liest den gesamten Inhalt einer Datei in einen String.
std::string read_file(const std::string& filename);


// Speichert den Inhalt in eine Datei.
void save_to_file(const std::string& filename, const std::string& content);

// Liest den Inhalt einer JSON-Datei und gibt das JSON-Objekt zurück.
nlohmann::json read_json_config(const std::string& filename);