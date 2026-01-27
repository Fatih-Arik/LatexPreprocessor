#pragma once

#include <string>
#include <vector>
#include <optional>
#include <filesystem>

/**
 * Struktur zur Speicherung der Kommandozeilen-Konfiguration.
 *
 * Diese Struktur repräsentiert alle relevanten Parameter, die über die
 * Kommandozeile an den Präprozessor übergeben werden können.
 */
struct CliConfig {

    /// Pfad zur Eingabedatei (Pflichtparameter)
    std::string input_file;

    /// Pfad zur Ausgabedatei (Standard: output.tex)
    std::string output_file = "output.tex";

    /// Zielformat der Ausgabe (z.B. "latex")
    std::string format = "latex";

    /// Pfad zur JSON-Datei mit Makrodefinitionen
    std::string macro_file = "macros.json";
};

/**
 * Liefert den Standardpfad zur Makro-Konfigurationsdatei.
 *
 * Wird verwendet, wenn kein expliziter Makropfad über die CLI angegeben wird.
 *
 * return Pfad zur Standard-Makrodatei
 */
std::filesystem::path get_default_macro_path();

/**
 * Liefert den Standardpfad für die Ausgabedatei.
 *
 * return Pfad zur Standard-Ausgabedatei
 */
std::filesystem::path get_default_output_path();

/**
 * Parst und validiert die Kommandozeilenargumente.
 *
 * Verwendet die cxxopts-Bibliothek zur Auswertung der übergebenen Parameter.
 * Bei fehlerhaften oder unvollständigen Argumenten wird std::nullopt
 * zurückgegeben.
 *
 * param argc Argumentanzahl
 * param argv Argumentwerte
 * return Optional gefüllte CliConfig-Struktur
 */
std::optional<CliConfig> parse_cli_args(int argc, char* argv[]);
