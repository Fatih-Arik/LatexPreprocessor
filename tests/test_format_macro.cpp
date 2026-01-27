#include <catch2/catch_test_macros.hpp>
#include "macro_utils.h"
#include "test_helper.h"

TEST_CASE("Formatmakro ignoriert echtes LaTeX") {
    PreprocReport report;

    std::string input = "\\frac{1}{2}";
    auto lines = make_lines(input);

    macro_spec spec{
        "\\frac", 2, "\\frac{__0__}{__1__}"
    };

    auto out = simplify_macro_spec(lines, spec, report);

    REQUIRE(out[0].line == "\\frac{1}{2}");
    REQUIRE_FALSE(report.has_errors());
}
