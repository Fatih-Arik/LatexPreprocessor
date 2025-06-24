#include <macro_handler.h>

#include "file_utils.h" 
#include "macro_utils.h"
#include "preprocessor.h"
#include <json.hpp>

#include <iostream>



/**
    Lädt alle dynamischen Makros aus einer JSON-Datei (z. B. dynamic_macro.json).

    Unterstützt folgende Typen: format, define, include, conditional, codeblock.

    Parameter: Pfad zur Konfigurationsdatei
    Rückgabe: Map vom Makronamen zum zugehörigen DynamicMacro-Eintrag.
*/
std::unordered_map<std::string, dynamic_macro> load_all_macros(const std::string& path) {
    std::unordered_map<std::string, dynamic_macro> result;
    nlohmann::json json_data = read_json_config(path);

    for (const auto& [name, entry] : json_data.items()) {
        
        dynamic_macro macro;
        macro.name = name;
        
        std::string type = entry["type"].get<std::string>();


        if (type == "format") {
            macro.type = macro_type::Format;
            macro.arg_count = entry["arg_count"].get<size_t>();
            macro.replacement = entry["replacement"].get<std::string>();
        }
        else if (type == "define") {
            macro.type = macro_type::Define;
        }
        else if (type == "include") {
            macro.type = macro_type::Include;
        }
        else if (type == "conditional") {
            macro.type = macro_type::Conditional;
        }
        else if (type == "codeblock") {
            macro.type = macro_type::Codeblock;
        }
        else {
            std::cerr << "Unbekannter makro_typ: " << type << "\n";
            continue;
        }
        result[name] = macro;
    }

    return result;
}

/**
    Wendet alle dynamischen Makros auf den Eingabetext an.

    Parameter: Eingabe Text mit den Makros
    Rückgabe: Ersetzter Text
*/
std::string apply_all_macros(
    const std::string& input_text,
    const std::unordered_map<std::string, dynamic_macro>& macros,
    const std::unordered_map<std::string, std::string>& defines)
{
    std::string result = input_text;

    // 1. Includes (können weitere Makros enthalten)
    if (macros.contains("\\include")) {
        result = process_include(result);
    }

    // 2. Defines entfernen
    if (macros.contains("\\define")) {
        result = remove_defines(result);
    }

    // 3. Bedingungen (\ifdef)
    if (!defines.empty() && macros.contains("\\ifdef")) {
        result = process_conditionals(result, defines);
        result = replace_text_macros(result, defines);
    }

    // 4. Codeblöcke (z. B. #codeblock[cpp])
    if (macros.contains("#codeblock")) {
        result = simplify_codeblocks(result);
    }

    // 5. Formatmakros wie \frac, \sqrt usw.
    for (const auto& [name, macro] : macros) {
        if (macro.type == macro_type::Format) {
            result = simplify_macro_spec(result, {
                macro.name, macro.arg_count, macro.replacement });
        }
    }

    // 6. Math-Wrappers (enthält oft weitere Makros)
    result = simplify_inline_math(result);
    result = simplify_block_math(result);

    return result;
}

