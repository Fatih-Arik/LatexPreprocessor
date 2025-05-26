#include "../include/file_utils.h"



#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem> 

// Liest den gesamten Inhalt einer Datei in einen String.
// Parameter: 
//   - filename: Pfad zur Datei (relativ oder absolut).
// Rückgabe: 
//   - Inhalt der Datei als String oder leerer String bei Fehler.

std::string read_file(const std::string& filename) {
    std::ifstream file(filename);  // Datei öffnen
    if (!file) {  // Falls die Datei nicht geöffnet werden kann
        std::cerr << "+++ Fehler: Datei konnte nicht geöffnet werden +++ : " << filename << "\n";
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();  // Inhalt in den Buffer laden
    return buffer.str();
}

// Speichert den Inhalt in eine Datei.
// Parameter: 
//   - filename: Pfad zur Zieldatei.
//   - content:  Zu speichernder Textinhalt.
// Rückgabe: 
//   - Keine Rückgabe. Gibt Erfolg oder Fehler über die Konsole aus.
void save_to_file(const std::string& filename, const std::string& content) {
    std::ofstream out(filename);
    if (!out) {
        std::cerr << "+++ Fehler: Datei konnte nicht gespeichert werden! +++" << "\n";
        return;
    }
    out << content;
    out.close();
    std::cout << "Datei gespeichert: " << filename << "\n";
}

// Liest den Inhalt einer JSON-Datei und gibt das JSON-Objekt zurück.
// 
// Parameter:
// - filename: Pfad zur Datei (relativ oder absolut)
//
// Rückgabe:
// - Ein nlohmann::json-Objekt mit dem Inhalt der Datei
//   (bei Fehler ein leeres JSON-Objekt)
nlohmann::json read_json_config(const std::string& filename) {

    std::filesystem::path config_path = std::filesystem::current_path() / filename;
    config_path = config_path.generic_string();
    std::cout << "Lade JSON aus: " << config_path << "\n";
    std::ifstream file(config_path);
    if (!file) {
        std::cerr << "+++ Fehler: Konnte: " << config_path.string() << "nicht öffnen! +++\n";
        return nlohmann::json();   // Gibt ein leeres JSON-Objekt zurück
    }
   
    nlohmann::json config;
    file >> config;  // Datei in JSON-Objekt einlesen
    
    return config;

}

// Liest den Inhalt von macros.json und gibt den Inhalt als unordered_map zurück
// Parameter:
// - filename: Pfad zur Datei (relativ oder absolut)
//
// Rückgabe:
// - Ein unordered_map-Objekt mit dem Inhalt der Datei als Key-Value paaren
std::unordered_map<std::string, MacroSpec> load_macros_from_file(const std::string& filename) {

    std::unordered_map<std::string, MacroSpec> macro_map;
    nlohmann::json macro_json;
    macro_json = read_json_config(filename);

    for (auto& [key, values] : macro_json.items()) {
        // Lese aus dem JSON-Array: erstes Element ist die Argumentanzahl (als size_t)
        // zweites Element ist das Ausgabeformat (als std::string)
        size_t arg_count = values[0].get<size_t>(); 
        std::string format = values[1].get<std::string>();
        macro_map[key] = { key, arg_count, format };
    }
    return macro_map;
}