#include <catch2/catch_test_macros.hpp>

#include "error_collector.h"
#include "preprocessor.h"
#include "test_helper.h"


TEST_CASE("process_include - zyklisches Include") {
    PreprocReport report;
    std::unordered_set<std::string> include_stack;
    // Simuliere read_file_lines() ggf. per Test-Dateien
    auto lines = make_lines("\\include{../latex_docs/recursive_content.tex}");

    auto result = process_include(lines, report, include_stack);

    REQUIRE(report.has_errors());
}
