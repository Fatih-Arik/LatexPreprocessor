#include "test_runner.h"
#include <iostream>
#include "../include/macro_utils.h" 
#include "../include/preprocessor.h"
#include "../include/file_utils.h"



void run_all_tests() {
    std::cout << "==== Starte Tests ====\n";

    // Hier einzelne Tests aufrufen
    // z.B.
    test_simplify_macro_spec();
    test_extract_math_args();
    test_codeblocks();
    test_simplify_inline_math();
    test_simplify_block_math();

    std::cout << "==== Alle Tests abgeschlossen ====\n";
}

// Hier erste einfache Tests bauen!
void test_simplify_macro_spec() {
    std::cout << "Teste simplify_macro_spec...\n";

    std::string input = "frac(1,2";
    MacroSpec spec = { "frac", 2, "\\frac{__0__}{__1__}" };
    std::string output = simplify_macro_spec(input, spec);

    if (output == "\\frac{1}{2}") {
        std::cout << "✅ simplify_macro_spec OK\n";
    }
    else {
        std::cout << "❌ simplify_macro_spec Fehler: " << output << "\n";
    }
}

void test_extract_math_args() {
    std::cout << "🔍 Teste extract_math_args...\n";

    struct TestCase {
        std::string input;
        size_t start_pos;
        std::vector<std::string> expected_args;
    };

    std::vector<TestCase> tests = {
        {"frac(1, 2)", 4, {"1", " 2"}},
        {"frac(1, sqrt(2))", 4, {"1", " sqrt(2)"}},
        {"sqrt(42)", 4, {"42"}},
        {"frac(1, frac(2, 3))", 4, {"1", " frac(2, 3)"}},
        {"frac(1, 2", 4, {}},
        {"frac1, 2)", 4, {}},
        {"frac(1, 2))", 4, {"1", " 2"}}
    };

    for (const auto& test : tests) {
        size_t end_pos;
        auto result = extract_math_args(test.input, test.start_pos, end_pos);

        std::cout << "Input: " << test.input << "\n";
        std::cout << "→ Args: ";
        for (const auto& arg : result) std::cout << "[" << arg << "] ";
        std::cout << "\n";
        std::cout << "End_Pos: " << end_pos << "\n";
    }

    std::cout << "✅ extract_math_args abgeschlossen.\n\n";
    
}
void test_codeblocks() {
    std::cout << "🔍 Teste simplify_codeblocks...\n";

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

        if (output == test.expected_output) {
            std::cout << "✅ Test OK: " << test.input.substr(0, 30) << "...\n";
        }
        else {
            std::cout << "❌ Fehler!\n";
            std::cout << "Input:    " << test.input << "\n";
            std::cout << "Erwartet: " << test.expected_output << "\n";
            std::cout << "Bekommen: " << output << "\n";
        }
    }

    std::cout << "✅ simplify_codeblocks abgeschlossen.\n\n";
}

void test_simplify_inline_math() {
    std::cout << "Teste simplify_inline_math...\n";

    std::string input = "#math(frac(1, 2))";
    std::string expected = "\\(\\frac{1}{ 2}\\)";
    std::string output = simplify_all_macros(input);

    if (output == expected)
        std::cout << "✅ Inline-Mathe OK\n";
    else
        std::cout << "❌ Inline-Mathe Fehler: " << output << "\n";
}

void test_simplify_block_math() {
    std::cout << "Teste simplify_block_math...\n";
    std::string input = "#blockmath(frac(1,abs(2)))";
    std::string expected = "\\[\\frac{1}{\\left|2\\right|}\\]";
    std::string output = simplify_all_macros(input);
    

    if (output == expected)
        std::cout << "✅ Block-Mathe OK\n";
    else
        std::cout << "❌ Block-Mathe Fehler: " << output << "\n";
}