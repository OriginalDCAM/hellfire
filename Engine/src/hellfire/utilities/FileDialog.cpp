//
// Created by denzel on 11/04/2025.
//

#include "hellfire/utilities/FileDialog.h"

#include <imgui.h>
#ifdef _WIN32
#include <windows.h>
#include <commdlg.h>
#endif

namespace hellfire::Utility {
    std::string FileDialog::win32_open_file(const std::vector<FileFilter> &filters) {
        std::string filepath;

        OPENFILENAMEA ofn;
        CHAR szFile[260] = {};
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = GetActiveWindow();
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);

        // Convert filters to Windows format
        std::string filterStr;
        if (filters.empty()) {
            // Default to all files if no filters provided
            filterStr = "All Files\0*.*\0";
        } else {
            for (const auto &filter: filters) {
                filterStr += filter.name + '\0' + filter.extensions + '\0';
            }
        }

        // Create a non-const copy that will persist for the duration of the call
        std::vector filterBuf(filterStr.begin(), filterStr.end());
        filterBuf.push_back('\0'); // Add final null terminator

        ofn.lpstrFilter = filterBuf.data();
        ofn.nFilterIndex = 1;
        ofn.lpstrFileTitle = NULL;
        ofn.nMaxFileTitle = 0;
        ofn.lpstrInitialDir = NULL;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

        if (GetOpenFileNameA(&ofn) == TRUE) {
            filepath = ofn.lpstrFile;
        }

        return filepath;
    }

    std::string FileDialog::win32_save_file(const std::string &default_filename,
                                            const std::vector<FileFilter> &filters) {
        std::string filepath;

        OPENFILENAMEA ofn;
        CHAR szFile[260] = {0};

        // Pre-populate with default filename if provided
        if (!default_filename.empty()) {
            strncpy(szFile, default_filename.c_str(), sizeof(szFile) - 1);
        }

        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = GetActiveWindow();
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);

        // Convert filters to Windows format
        std::string filterStr;
        std::string defaultExt;

        if (filters.empty()) {
            // Default to all files if no filters provided
            filterStr = "All Files\0*.*\0";
        } else {
            for (const auto &filter: filters) {
                filterStr += filter.name + '\0' + filter.extensions + '\0';

                // Set default extension from first filter if not already set
                if (defaultExt.empty() && !filter.extensions.empty()) {
                    // Extract extension from the first extension in the list
                    size_t pos = filter.extensions.find("*.");
                    if (pos != std::string::npos) {
                        size_t endPos = filter.extensions.find(';', pos);
                        if (endPos == std::string::npos) {
                            endPos = filter.extensions.size();
                        }
                        // Get extension without the "*."
                        defaultExt = filter.extensions.substr(pos + 2, endPos - pos - 2);
                    }
                }
            }
        }

        // Create a non-const copy that will persist for the duration of the call
        std::vector<char> filterBuf(filterStr.begin(), filterStr.end());
        filterBuf.push_back('\0'); // Add final null terminator

        ofn.lpstrFilter = filterBuf.data();
        ofn.nFilterIndex = 1;
        ofn.lpstrFileTitle = NULL;
        ofn.nMaxFileTitle = 0;
        ofn.lpstrInitialDir = NULL;
        ofn.Flags = OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

        // Set default extension if we found one
        if (!defaultExt.empty()) {
            // Windows API requires a non-const char*
            std::vector<char> extBuf(defaultExt.begin(), defaultExt.end());
            extBuf.push_back('\0');
            ofn.lpstrDefExt = extBuf.data();
        }

        if (GetSaveFileNameA(&ofn) == TRUE) {
            filepath = ofn.lpstrFile;
        }

        return filepath;
    }

    std::string FileDialog::imgui_open_file(const std::vector<FileFilter> &filters) {
        static std::string filepath;
        filepath.clear();

        // Format filter description for display
        std::string filterDesc = "Supported formats: ";
        if (filters.empty()) {
            filterDesc += "All files";
        } else {
            for (size_t i = 0; i < filters.size(); ++i) {
                filterDesc += filters[i].name;
                if (i < filters.size() - 1) {
                    filterDesc += ", ";
                }
            }
        }

        ImGui::OpenPopup("Open File");

        if (ImGui::BeginPopupModal("Open File", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            static char buf[512] = "assets/";
            ImGui::Text("%s", filterDesc.c_str());
            ImGui::InputText("Path to file", buf, 512);

            if (ImGui::Button("OK", ImVec2(120, 0))) {
                filepath = buf;
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0))) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        return filepath;
    }

    std::string FileDialog::imgui_save_file(const std::string &default_filename,
                                            const std::vector<FileFilter> &filters) {
        static std::string filepath;
        filepath.clear();

        // Format filter description for display
        std::string filterDesc = "Save as: ";
        if (filters.empty()) {
            filterDesc += "All files";
        } else {
            for (size_t i = 0; i < filters.size(); ++i) {
                filterDesc += filters[i].name;
                if (i < filters.size() - 1) {
                    filterDesc += ", ";
                }
            }
        }

        ImGui::OpenPopup("Save File");

        if (ImGui::BeginPopupModal("Save File", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            static char buf[512] = "";

            // Initialize with default filename if empty
            if (buf[0] == '\0' && !default_filename.empty()) {
                strncpy(buf, default_filename.c_str(), sizeof(buf) - 1);
            }

            ImGui::Text("%s", filterDesc.c_str());
            ImGui::InputText("Filename", buf, 512);

            if (ImGui::Button("Save", ImVec2(120, 0))) {
                filepath = buf;

                // Extract default extension from first filter if we need to add one
                if (!filters.empty() && filepath.find('.') == std::string::npos) {
                    const auto &firstFilter = filters[0];
                    size_t pos = firstFilter.extensions.find("*.");
                    if (pos != std::string::npos) {
                        size_t endPos = firstFilter.extensions.find(';', pos);
                        if (endPos == std::string::npos) {
                            endPos = firstFilter.extensions.size();
                        }
                        std::string ext = firstFilter.extensions.substr(pos + 1, endPos - pos - 1);
                        filepath += ext;
                    }
                }

                ImGui::CloseCurrentPopup();
            }

            ImGui::SameLine();

            if (ImGui::Button("Cancel", ImVec2(120, 0))) {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        return filepath;
    }


    std::string FileDialog::open_file(const std::vector<FileFilter> &filters) {
#ifdef _WIN32
        return win32_open_file(filters);
#else
        return imgui_open_file(filters);
#endif
    }

    std::string FileDialog::save_file(const std::string &default_filename,
                                      const std::vector<FileFilter> &filters) {
#ifdef _WIN32
        return win32_save_file(default_filename, filters);
#else
        return imgui_save_file(default_filename, filters);
#endif
    }
}
