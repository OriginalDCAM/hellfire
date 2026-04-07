//
// Created by denzel on 11/04/2025.
//
#pragma once
#include <string>
#include <vector>

namespace hellfire::Utility {

    struct FileFilter {
        std::string name;
        std::string extensions; 
    };

    class FileDialog {
    public:
        static std::string open_file(const std::vector<FileFilter>& filters = {});
        static std::string save_file(std::string& save_name_to,
            const std::string& default_filename = "", 
                                     const std::vector<FileFilter>& filters = {});

    private:
        // Platform-specific implementations
        static std::string win32_open_file(const std::vector<FileFilter>& filters);
        static std::string win32_save_file(const std::string &default_filename,
                                           const std::vector<FileFilter> &filters, std::string &save_name_to);
        static std::string imgui_open_file(const std::vector<FileFilter>& filters);
        static std::string imgui_save_file(const std::string& default_filename, 
                                          const std::vector<FileFilter>& filters);
    };

} // namespace hellfire::Utility
