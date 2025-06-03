#include "test_runner.h"
#include <iostream>
#include <cassert>

#include "../include/macro_utils.h" 
#include "../include/preprocessor.h"
#include "../include/file_utils.h"



void run_all_tests() {
    std::cout << "==== Starte Tests ====\n";

 
    test_simplify_macro_spec();
    test_extract_math_args();
    test_codeblocks();
    test_simplify_inline_math();
    test_simplify_block_math();
    test_ifdef();
    std::cout << "==== Alle Tests abgeschlossen ====\n";
}


void test_simplify_macro_spec() {
    // Test: Ungültige Makros sollen nicht verändert werden (z. B. fehlende schließende Klammer)
    std::string input = "\\frac(1,2";
    MacroSpec spec = { "\\frac", 2, "\\frac{__0__}{__1__}" };
    std::string output = simplify_macro_spec(input, spec);
    
    assert(output == "\\frac(1,2" && "simplify_macro_spec fehlgeschlagen.");
    
}

void test_extract_math_args() {
    std::cout << "Teste extract_math_args...\n";

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
        size_t end_pos;
        auto result = extract_math_args(test.input, test.start_pos, end_pos);
        for (auto& value : result){
            std::cout << "result: " << value << "\n";
        }
        assert(result == test.expected_args && "extract_math_args fehlgeschlagen");
    }

    std::cout << "extract_math_args abgeschlossen.\n\n";
    
}
void test_codeblocks() {
    std::cout << "Teste simplify_codeblocks...\n";

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

        assert(output == test.expected_output && "simplify_codeblocks fehlgeschlagen");
    }

    std::cout << "simplify_codeblocks abgeschlossen.\n\n";
}

void test_simplify_inline_math() {
    std::cout << "Teste simplify_inline_math...\n";

    std::string macro_path = "./config/macros.json";
    std::string input = "#math(\\frac(1, 2))";
    std::string expected = "\\(\\frac{1}{ 2}\\)";
    std::string output = simplify_all_macros(input, macro_path);
    assert(output == expected && "simplify_inline_math fehlgeschlagen");
}

void test_simplify_block_math() {
    std::string macro_path = "./config/macros.json";
    std::string input = "#blockmath(\\frac(1,\\abs(2)))";
    std::string expected = "\\[\\frac{1}{\\left|2\\right|}\\]";
    std::string output = simplify_all_macros(input, macro_path);
    assert(output == expected && "simplify_block_math fehlgeschlagen");
}


void test_ifdef() {
    std::unordered_map<std::string, std::string> defines;

    // Test 1: define vorhanden
    defines["DEBUG"] = "";
    std::string input = "\\ifdef{DEBUG}\nSichtbar\n\\endif\n";
    std::string expected = "Sichtbar\n";
    std::string result = process_conditionals(input, defines);
    assert(result == expected && "ifdef-Block sollte sichtbar sein");

    // Test 2: define fehlt
    defines.clear();
    input = "\\ifdef{DEBUG}\nNicht sichtbar\n\\endif\n";
    expected = "";
    result = process_conditionals(input, defines);
    assert(result == expected && "ifdef-Block sollte nicht sichtbar sein");

    // Test 3: \else-Block
    input = "\\ifdef{DEBUG}\nNicht sichtbar\n\\else\nSichtbar\n\\endif\n";
    expected = "Sichtbar\n";
    result = process_conditionals(input, defines);
    assert(result == expected && "else-Block sollte sichtbar sein");

    std::cout << "Alle ifdef-Tests erfolgreich bestanden!" << std::endl;
}
