#include "file_utils.h"



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
    
    std::filesystem::path output_path(filename);
    // Ordner automatisch erzeugen, falls nicht vorhanden
    try {
        std::filesystem::create_directories(output_path.parent_path());
    }
    catch (const std::exception& e) {
        std::cerr << "+++ Fehler beim Erstellen der Verzeichnisse: " << e.what() << " +++\n";
    }
    std::ofstream out(filename);
    if (!out) {
        std::cerr << "+++ Fehler beim Öffnen der Datei zum Schreiben: " << filename << "+++\n";
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

    std::cout << "Lade JSON aus: " << filename << "\n";
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "+++ Fehler: Konnte: " << filename << "sein +++\n";
        return nlohmann::json();   // Gibt ein leeres JSON-Objekt zurück
    }
    try {
        nlohmann::json config;
        file >> config;  // Datei in JSON-Objekt einlesen
        return config;
    }
    catch (const std::exception& e) {
        std::cerr << "+++ Fehler beim Parsen der JSON-Datei: " << e.what() << "\n";
        return nlohmann::json();
    }
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