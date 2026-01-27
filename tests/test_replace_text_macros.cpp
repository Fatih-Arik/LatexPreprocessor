#include <catch2/catch_test_macros.hpp>

#include "preprocessor.h"
#include "test_helper.h"

#include <string>
#include <unordered_map>

TEST_CASE("replace_text_macros - exakte Ersetzung") {
    auto lines = make_lines("AUTHOR_NAME AUTHOR");
    std::unordered_map<std::string, std::string> defs = {
        {"AUTHOR", "Fatih"}
    };

    auto result = replace_text_macros(lines, defs);

    REQUIRE(join_lines(result) == "AUTHOR_NAME Fatih\n");
}

TEST_CASE("replace_text_macros - Wortgrenzen") {
    auto lines = make_lines("NAME NAME1 _NAME NAME_");
    std::unordered_map<std::string, std::string> defs = {
        {"NAME", "X"}
    };

    auto result = replace_text_macros(lines, defs);

    REQUIRE(join_lines(result) == "X NAME1 _NAME NAME_\n");
}