#pragma once

#include "source_line.h"
#include <vector>
#include <sstream>
#include <string>

static std::vector<SourceLine> make_lines(
    const std::string& text,
    const std::string& file = "test.tex")
{
    std::vector<SourceLine> lines;
    std::istringstream iss(text);
    std::string line;
    int ln = 1;

    while (std::getline(iss, line)) {
        lines.push_back({ line, file, ln++ });
    }
    return lines;
}

static std::string join_lines(const std::vector<SourceLine>& lines) {
    std::ostringstream out;
    for (const auto& l : lines) {
        out << l.line << "\n";
    }
    return out.str();
}