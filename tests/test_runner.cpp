#include <catch2/catch_test_macros.hpp>

#include <iostream>
#include <cassert>
#include <unordered_map>
#include <string>
#include <vector>
#include <filesystem>


#include "macro_utils.h" 
#include "preprocessor.h"
#include "file_utils.h"
#include "cli_utils.h"



TEST_CASE("simplify_macro_spec") {
    std::string input = "\\frac(1,2";
    macro_spec spec = { "\\frac", 2, "\\frac{__0__}{__1__}" };
    std::string output = simplify_macro_spec(input, spec);
    REQUIRE(output != "\\frac{1}{2}");
}


TEST_CASE("extract_math_args") {
    struct TestCase {
        std::string input;
        size_t start_pos;
        std::vector<std::string> expected_args;
    };

    std::vector<TestCase> tests = {
        {"\\frac(1, 2)", 5, {"1", " 2"}},
        {"\\sqrt(42)", 5, {"42"}},
        {"\\frac(1, \\sqrt(2))", 5, {"1", " \\sqrt(2)"}},
    };

    for (const auto& test : tests) {
        size_t end_pos = 0;
        auto result = extract_math_args(test.input, test.start_pos, end_pos);
        REQUIRE(result == test.expected_args);
    }
}

TEST_CASE("replace_text_macros") {
    SECTION("Einfacher Makroersatz") {
        std::string text = "Hallo NAME";
        std::unordered_map<std::string, std::string> macros = { {"NAME", "Max"} };
        
        REQUIRE(replace_text_macros(text, macros) == "Hallo Max");
    }

    SECTION("Makro nicht in zusammengesetzten Wörtern ersetzen") {
        std::string text = "USERNAME";
        std::unordered_map<std::string, std::string> macros = { {"NAME", "Max"} };
        REQUIRE(replace_text_macros(text, macros) == "USERNAME");
    }

    SECTION("Makro ist escaped") {
        std::string text = "Hallo \\NAME";
        std::unordered_map<std::string, std::string> macros = { {"NAME", "Max"} };

        // Aktuelles Verhalten: \NAME wird zu \Max
        REQUIRE(replace_text_macros(text, macros) == "Hallo \\Max");
    }

    SECTION("Mehrere Makros ersetzen") {
        std::string text = "Hallo NAME, Autor: AUTHOR";
        std::unordered_map<std::string, std::string> macros = {
            {"NAME", "Max"},
            {"AUTHOR", "Fatih"}
        };
        REQUIRE(replace_text_macros(text, macros) == "Hallo Max, Autor: Fatih");
    }
}

TEST_CASE("simplify_codeblocks") {
    struct TestCase {
        std::string input;
        std::string expected_output;
    };

    std::vector<TestCase> tests = {
        {
            "#codeblock[cpp]{int main() { return 0; }}",
            "\\begin{lstlisting}[language=cpp]\nint main() { return 0; }\n\\end{lstlisting}"
        },
        {
            "#codeblock[python]{print('Hello, World!')}",
            "\\begin{lstlisting}[language=python]\nprint('Hello, World!')\n\\end{lstlisting}"
        },
        {
            "Etwas Text davor\n#codeblock[cpp]{int a = 42;}\nText danach",
            "Etwas Text davor\n\\begin{lstlisting}[language=cpp]\nint a = 42;\n\\end{lstlisting}\nText danach"
        }
    };

    for (const auto& test : tests) {
        std::string output = simplify_codeblocks(test.input);
        REQUIRE(output == test.expected_output);
    }
}


TEST_CASE("simplify_inline_math") {
    std::string macro_path = get_default_macro_path().string();
    std::string input = "#math(\\frac(1, 2))";
    std::string expected = "\\(\\frac(1, 2)\\)";
    std::string output = simplify_inline_math(input);
    REQUIRE(output == expected);
}

TEST_CASE("extract_define") {
    SECTION("Makro nur mit Key") {
        std::string input = "\\define{DEBUG}";
        std::unordered_map<std::string, std::string> expected = { { "DEBUG", "" } };
        auto result = extract_defines(input);
        REQUIRE(result == expected);
    }

    SECTION("Makro mit Key und Wert") {
        std::string input = "\\define{AUTHOR}{Fatih}";
        std::unordered_map<std::string, std::string> expected = { { "AUTHOR", "Fatih" } };
        auto result = extract_defines(input);
        REQUIRE(result == expected);
    }

    SECTION("Mehrere Makros") {
        std::string input = "\\define{ A }{1}\n\\define{ B }{2}";
        std::unordered_map<std::string, std::string> expected = { {" A ", "1"}, {" B ", "2"} };
        auto result = extract_defines(input);
        REQUIRE(result == expected);
    }

    SECTION("Fehlerhafte Zeile wird ignoriert") {
        std::string input = "\\define{BROKEN";
        auto result = extract_defines(input);
        REQUIRE(result.empty());
    }

    SECTION("nicht Define Zeile") {
        std::string input = "Das ist ein normaler Satz.";
        auto result = extract_defines(input);
        REQUIRE(result.empty());
    }

}

TEST_CASE("process_conditionals - ifdef") {
    std::unordered_map<std::string, std::string> defines;

    SECTION("Makro vorhanden") {
        defines["DEBUG"] = "";
        std::string input = "\\ifdef{DEBUG}\nSichtbar\n\\endif\n";
        std::string expected = "Sichtbar\n";
        std::string result = process_conditionals(input, defines);
        REQUIRE(result == expected);
    }

    SECTION("Makro fehlt") {
        defines.clear();
        std::string input = "\\ifdef{DEBUG}\nNicht sichtbar\n\\endif\n";
        std::string expected = "";
        std::string result = process_conditionals(input, defines);
        REQUIRE(result == expected);
    }

    SECTION("Mit else") {
        defines.clear();
        std::string input = "\\ifdef{DEBUG}\nNicht sichtbar\n\\else\nSichtbar\n\\endif\n";
        std::string expected = "Sichtbar\n";
        std::string result = process_conditionals(input, defines);
        REQUIRE(result == expected);
    }
}


