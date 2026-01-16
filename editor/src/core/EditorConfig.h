//
// Created by denzel on 29/12/2025.
//

#pragma once
#include <filesystem>

namespace hellfire::editor {
    struct EditorConfig {
        static std::filesystem::path get_config_path() {
#ifdef _WIN32
            std::filesystem::path config_path = std::filesystem::path(getenv("APPDATA")) / "Hellfire";
#else
            std::filesystem::path config_path = std::filesystem::path(getenv("HOME")) / ".config" / "hellfire";
#endif
            return config_path;
        }
    };
}
