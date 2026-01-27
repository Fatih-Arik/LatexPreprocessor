#pragma once

#include <vector>
#include <string>


struct PreprocError {
    std::string file;
    std::string message;
    int line = -1; // optional
};


struct PreprocReport {
    std::vector<PreprocError> errors;

    bool has_errors() const { return !errors.empty(); }
};