#pragma once


// Diese Header-Datei enthält Deklarationen für Funktionen zur Verarbeitung
// von LaTeX-ähnlichen Makros. Die Funktionen erlauben das Einlesen, Ersetzen
// und Entfernen von Makros sowie das Verarbeiten von `#include`-Anweisungen.

#include <string>
#include <unordered_map>



//Ersetzt rekursiv alle \include{...}-Anweisungen im gegebenen Text durch den Inhalt der referenzierten Dateien.
std::string process_include(const std::string& content);


// Extrahiert alle `\define`-Makros aus dem Quelltext und speichert sie als Schlüssel - Wert - Paare.
std::unordered_map<std::string, std::string> extract_defines(const std::string& content);


// Ersetzt im Text alle Makronamen durch ihre zugehörigen Werte aus der Makro-Tabelle.
std::string replace_text_macros(const std::string& text, const std::unordered_map<std::string, std::string>& macros);


//Entfernt alle `\define`-Makros aus dem Text.
std::string remove_defines(const std::string& text);


// Verarbeitet \ifdef-Blöcke mit optionalem \else.
std::string process_conditionals(const std::string& text, const std::unordered_map<std::string, std::string>& defines);


// Extrahiert die Programmiersprache aus einem Codeblock-Makro
std::string extract_language(const std::string& text, size_t start_pos, size_t& lang_end_pos);


// Extrahiert den Inhalt eines Codeblocks aus einem Makro
std::string extract_codeblock_body(const std::string& text, size_t start_pos, size_t& end_pos);


//Wandelt alle #codeblock[language]{ ... } - Makros in LaTeX \lstlisting - Umgebungen um.
std::string simplify_codeblocks(const std::string& input);