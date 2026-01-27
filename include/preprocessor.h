#pragma once


/**
 * preprocessor.h
 * Deklarationen zentraler Präprozessor-Funktionen für LaTeX-artige Makros.
 *
 * Dieses Modul enthält Funktionen zur Vorverarbeitung von Textdateien,
 * einschließlich Dateiinklusionen, Makroextraktion, Makroersetzung und
 * bedingter Textverarbeitung. Die Implementationen befinden sich in
 * preprocessor.cpp.
 */
#include "error_collector.h"
#include "source_line.h"

#include <string>
#include <unordered_map>
#include <unordered_set>


 /**
  * Ersetzt rekursiv alle \include{...}-Anweisungen durch den Inhalt
 * der jeweils referenzierten Datei.
 *
 * Die Verarbeitung erfolgt zeilenbasiert auf SourceLine-Ebene, sodass
 * Dateiname und originale Zeilennummern erhalten bleiben.
 *
 * Semantik:
 *   \include{file.tex}
 * wird ersetzt durch:
 *   [Inhalt von file.tex]
 *
 * Fehler (z. B. Syntaxfehler oder fehlende Dateien) werden im
 * PreprocReport gesammelt und abbrechfrei behandelt.
 */
  
std::vector<SourceLine> process_include(const std::vector<SourceLine>& content,
    PreprocReport& report,
    std::unordered_set<std::string>& include_stack
);


/**
 * Extrahiert alle \define-Makros aus dem Text.
 *
 * Beispiele:
 *   \define{AUTHOR}{Max}
 *   \define{DEBUG}
 *
 * Rückgabe ist eine Map KEY → VALUE (leerer Wert für flag-artige Defines).
 *
 * Syntaxfehler (fehlende Klammern etc.) werden in report eingetragen.
 */
std::unordered_map<std::string, std::string> extract_defines(const std::vector<SourceLine>& content, PreprocReport& report);


/**
 * Ersetzt alle vorkommenden Makro-Schlüssel durch ihre Werte.
 *
 * Die Ersetzung folgt exakten Textübereinstimmungen. 
 *
 * Beispiel:
 *   defines: NAME -> "Max"
 *   "Hallo NAME" -> "Hallo Max"
 */
std::vector<SourceLine> replace_text_macros(const std::vector<SourceLine>& text, const std::unordered_map<std::string, std::string>& macros);



/**
 * Entfernt sämtliche \define-Anweisungen aus dem Quelltext.
 */
std::vector<SourceLine> remove_defines(const std::vector<SourceLine>& content);

/**
 * Verarbeitet \ifdef-Blöcke mit optionalem \else.
 *
 * Einschränkungen:
 * - Keine Verschachtelung von \ifdef-Blöcken erlaubt
 * - Bedingungen prüfen ausschließlich auf Existenz in `defines`
 */
std::vector<SourceLine> process_conditionals(const std::vector<SourceLine>& text, const std::unordered_map<std::string, std::string>& defines, PreprocReport& report);
