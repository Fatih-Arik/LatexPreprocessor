# ============================================================
# Mindestversion von CMake
# ============================================================
cmake_minimum_required(VERSION 3.16)

# ============================================================
# Projektdefinition
# ============================================================
project(LatexPreprocessor
    VERSION 1.0
    LANGUAGES CXX
)

# ============================================================
# Globale Compile-Definitionen
# ============================================================

# PROJECT_DIR verweist zur Compile-Zeit auf die Projektwurzel
# und wird im Code z. B. für Default-Pfade verwendet
add_compile_definitions(PROJECT_DIR="${CMAKE_SOURCE_DIR}")

# ============================================================
# C++-Standard
# ============================================================
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ============================================================
# Haupt-Executable
# ============================================================
add_executable(latexprepro
    src/main.cpp
    src/cli_utils.cpp
    src/file_utils.cpp
    src/macro_utils.cpp
    src/preprocessor.cpp
    src/macro_handler.cpp
)

# Include-Verzeichnisse gezielt pro Target setzen
target_include_directories(latexprepro
    PRIVATE
        include
)

# Compiler-Warnungen (nur für GCC / Clang)
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(latexprepro PRIVATE
        -Wall
        -Wextra
        -pedantic
    )
endif()

# ============================================================
# OPTIONALE TESTS
# ============================================================
option(BUILD_TESTS "Build unit tests" ON)

if (BUILD_TESTS)
    enable_testing()

    include(FetchContent)

    FetchContent_Declare(
        catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.8.1
    )

    FetchContent_MakeAvailable(catch2)

    add_executable(test_runner
        tests/test_conditionals.cpp
        tests/test_defines.cpp
        tests/test_format_macro.cpp
        tests/test_include.cpp
        tests/test_replace_text_macros.cpp

        # Produktionscode wird wiederverwendet
        src/cli_utils.cpp
        src/file_utils.cpp
        src/macro_utils.cpp
        src/preprocessor.cpp
        src/macro_handler.cpp
    )

    target_include_directories(test_runner
        PRIVATE
            include
    )

    target_link_libraries(test_runner
        PRIVATE
            Catch2::Catch2WithMain
    )

    if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(test_runner PRIVATE
            -Wall
            -Wextra
            -pedantic
        )
    endif()

    add_test(NAME unit_tests COMMAND test_runner)
endif()
