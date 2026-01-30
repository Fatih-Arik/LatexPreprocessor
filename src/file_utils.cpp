#include "file_utils.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem> 


/**
 * Liest eine Textdatei zeilenweise ein und annotiert jede Zeile
 * mit ihrer Herkunft (Dateiname und originale Zeilennummer).
 *
 * Diese Funktion bildet die Grundlage für eine präzise Fehlerdiagnose,
 * da jede Textzeile ihre Ursprungsdatei sowie die exakte Position im
 * Quelldokument beibehält. Sie wird insbesondere für die Verarbeitung
 * von \include-Anweisungen verwendet.
 *
 * Parameter:
 *   filename – Pfad zur Eingabedatei (relativ oder absolut)
 *
 * Rückgabe:
 *   Ein Vektor von Datentyp SourceLine, wobei jedes Element:
 *     - den Text der Zeile,
 *     - den Dateinamen der Quelldatei,
 *     - sowie die originale Zeilennummer enthält.
 *
 *   Falls die Datei nicht geöffnet werden kann, wird ein leerer Vektor
 *   zurückgegeben und ein Fehler auf stderr ausgegeben.
 */
std::vector<SourceLine> read_file_lines(const std::string& filename) {

    std::vector<SourceLine> result;
    std::ifstream file(filename);  // Datei öffnen
    if (!file) {  // Falls die Datei nicht geöffnet werden kann
        std::cerr << "+++ Fehler: Datei konnte nicht geöffnet werden +++ : " << filename << "\n";
        return result;
    }


    std::string line;
    int line_no = 1;

    while (std::getline(file, line)) {
        result.push_back({
            line,
            filename,
            line_no++
        });
    }

    return result;
}


// Speichert den Inhalt in eine Datei.
// Parameter: 
//   - filename: Pfad zur Zieldatei.
//   - content:  Zu speichernder Textinhalt.
// Rückgabe: 
//   - Keine Rückgabe. Gibt Erfolg oder Fehler über die Konsole aus.
void save_to_file(const std::string& filename, const std::vector<SourceLine>& content) {
    
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
    
    for (const SourceLine& sl : content) {
        out << sl.line << '\n';
    }

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
        std::cerr << "[Fehler] Makrodatei nicht gefunden: " << filename << "\n";
        return nlohmann::json();   // Gibt ein leeres JSON-Objekt zurück
    }
    try {
        nlohmann::json config;
        file >> config;  // Datei in JSON-Objekt einlesen
        return config;
    }
    catch (const std::exception& e) {
        std::cerr << "[Fehler] Ungültige JSON-Datei: " << e.what() << "\n";
        return nlohmann::json();
    }
}
