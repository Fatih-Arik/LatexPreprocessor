# Mindestversion von CMake, die für dieses Projekt benötigt wird.
cmake_minimum_required(VERSION 3.16)

# Projektname und interne Buildsystem-Version
project(LatexPreprocessor VERSION 1.0 LANGUAGES CXX)


# Definiert zur Kompilierzeit eine globale Konstante PROJECT_DIR
# Wird im C++-Code als Pfad zur Projektwurzel verwendet
add_compile_definitions(PROJECT_DIR="${CMAKE_SOURCE_DIR}")


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
    src/macro_handler.cpp
)

# Compilerwarnungen für GCC und Clang aktivieren 
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(latexprepro PRIVATE -Wall -Wextra -pedantic)
endif()





# ============================================================
# OPTIONALER TEST-BUILD (wird nur gebaut, wenn BUILD_TESTS=ON)
# ============================================================

# Definiert eine Option namens BUILD_TESTS.
# Diese kann beim Aufruf von CMake aktiviert oder deaktiviert werden.
# Standardmäßig ist sie "on", also werden Tests standardmäßig mitgebaut.
option(BUILD_TESTS "Build test suite" on)

if (BUILD_TESTS)
    # Aktiviert die CMake-Testfunktionalität (z. B. für 'ctest').
    enable_testing()

    # Deklariert das Catch2-Projekt, das per Git heruntergeladen werden soll.
    include(FetchContent)
    FetchContent_Declare(
      catch2
      GIT_REPOSITORY https://github.com/catchorg/Catch2.git
      GIT_TAG v3.5.2
    )
    # Lädt und macht das Catch2-Projekt verfügbar.
    FetchContent_MakeAvailable(catch2)

    # Erstellt das Test-Executable namens test_runner mit den angegebenen Quellcodedateien.
    add_executable(test_runner
        tests/test_runner.cpp
        src/cli_utils.cpp
        src/converter.cpp
        src/file_utils.cpp
        src/macro_utils.cpp
        src/preprocessor.cpp
        src/macro_handler.cpp
    )
    #Legt fest, dass der `include/`-Ordner für den Compiler sichtbar ist.
    target_include_directories(test_runner PRIVATE include)

    # Verlinkt das Catch2-Test-Framework mit dem Test-Executable.
    target_link_libraries(test_runner PRIVATE Catch2::Catch2WithMain)

    # Wenn der verwendete Compiler GCC oder Clang ist
    if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(test_runner PRIVATE -Wall -Wextra -pedantic)
    endif()

    # Fügt den Test dem CMake-Testsystem hinzu (ausführbar über `ctest`).
    add_test(NAME unit_tests COMMAND test_runner)
endif()