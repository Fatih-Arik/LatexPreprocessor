#include <catch2/catch_test_macros.hpp>
#include "error_collector.h"
#include "preprocessor.h"
#include "source_line.h"
#include "test_helper.h"
#include <vector>


#include <iostream>

TEST_CASE("extract_defines - gültige Defines") {
    PreprocReport report;

    std::vector<SourceLine> input = {
        {
            "\\define{DEBUG}",
            "test.txt",
            0
        },
        {
            "\\define{AUTHOR}{Fatih}",
            "test.txt",
            1
        }
    };

    auto defines = extract_defines(input, report);

    REQUIRE(defines.size() == 2);
    REQUIRE(defines["DEBUG"] == "");
    REQUIRE(defines["AUTHOR"] == "Fatih");
    REQUIRE_FALSE(report.has_errors());
}

TEST_CASE("extract_defines - Syntaxfehler") {
    PreprocReport report;


    std::string input = "\\define{BROKEN{X}";

    auto line = make_lines(input);
    auto defines = extract_defines(line, report);

    
    REQUIRE(defines.empty());
    REQUIRE(report.has_errors());
}

TEST_CASE("remove_defines entfernt Define-Zeilen") {
    std::string input = "\\define{DEBUG}\n" 
                        "Text\n"
                        "\\define{A}{1}";
    auto line = make_lines(input);
    std::vector<SourceLine> expected = { { "Text", "test.tex", 2 } };


    std::vector<SourceLine> removed = remove_defines(line);

    REQUIRE(removed == expected);
}

