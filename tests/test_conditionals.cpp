#include <catch2/catch_test_macros.hpp>

#include "error_collector.h"
#include "preprocessor.h"
#include "test_helper.h"


#include <string>
#include <unordered_map>

TEST_CASE("process_conditionals - ifdef true") {
    PreprocReport report;
    std::unordered_map<std::string, std::string> defs{
        {"DEBUG", ""}
    };

    std::string input =
        "\\ifdef{DEBUG}\n"
        "OK\n"
        "\\endif\n";

    auto lines = make_lines(input);
    auto result = process_conditionals(lines, defs, report);

    REQUIRE(join_lines(result) == "OK\n");
    REQUIRE_FALSE(report.has_errors());
}

TEST_CASE("process_conditionals - fehlendes endif") {
    PreprocReport report;
    std::unordered_map<std::string, std::string> defs{
        {"DEBUG", ""}
    };

    std::string input =
        "\\ifdef{DEBUG}\n"
        "OK\n";

    auto lines = make_lines(input);
    auto result = process_conditionals(lines, defs, report);

    REQUIRE(report.has_errors());
}