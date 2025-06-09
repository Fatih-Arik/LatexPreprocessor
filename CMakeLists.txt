# Mindestversion von CMake, die für dieses Projekt benötigt wird.
cmake_minimum_required(VERSION 3.16)

# Projektname und interne Buildsystem-Version
project(LatexPreprocessor VERSION 1.0 LANGUAGES CXX)

# C++20 aktivieren (erforderlich für std::filesystem und modernes C++)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)  # Keine Compiler-spezifischen Erweiterungen

# Eigenen Include-Ordner angeben 
include_directories(include)

# Alle Quellcode-Dateien für das Hauptprogramm (Produktionscode)
add_executable(latexprepro
    src/main.cpp
    src/cli_utils.cpp
    src/converter.cpp
    src/file_utils.cpp
    src/macro_utils.cpp
    src/preprocessor.cpp
)

# Compilerwarnungen für GCC und Clang aktivieren 
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(latexprepro PRIVATE -Wall -Wextra -pedantic)
endif()





# ============================================================
# OPTIONALER TEST-BUILD (wird nur gebaut, wenn BUILD_TESTS=ON)
# ============================================================

# Option definieren (ON bedeutet: standardmäßig Tests bauen)
option(BUILD_TESTS "Build test suite" on)

if (BUILD_TESTS)
    enable_testing()

    include(FetchContent)
    FetchContent_Declare(
      catch2
      GIT_REPOSITORY https://github.com/catchorg/Catch2.git
      GIT_TAG v3.5.2
    )
    FetchContent_MakeAvailable(catch2)

    add_executable(test_runner
        tests/test_runner.cpp
        src/cli_utils.cpp
        src/converter.cpp
        src/file_utils.cpp
        src/macro_utils.cpp
        src/preprocessor.cpp
    )

    target_include_directories(test_runner PRIVATE include)
    target_link_libraries(test_runner PRIVATE Catch2::Catch2WithMain)

    if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(test_runner PRIVATE -Wall -Wextra -pedantic)
    endif()

    add_test(NAME unit_tests COMMAND test_runner)
endif()