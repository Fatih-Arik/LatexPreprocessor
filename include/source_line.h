#pragma once

#include <string>

struct SourceLine {
    std::string line;   // Inhalt der Zeile
    std::string file;   // Quelldatei
    int line_nr;        // Original-Zeilennummer

    bool operator==(const SourceLine& b) const{
        return file == b.file
            && line_nr == b.line_nr
            && line == b.line;
    }
};



