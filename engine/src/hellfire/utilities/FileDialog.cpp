//
// Created by denzel on 11/04/2025.
//

#include "hellfire/utilities/FileDialog.h"

#include <filesystem>
#include <imgui.h>
#include <iostream>
#ifdef _WIN32
#include <windows.h>
#include <commdlg.h>
#include <shobjidl.h>
#include <shlobj.h>
#endif

namespace hellfire::Utility {
    std::string FileDialog::win32_open_file(const std::vector<FileFilter> &filters,
                                            const std::filesystem::path &default_path) {
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
        ofn.lpstrFileTitle = nullptr;
        ofn.nMaxFileTitle = 0;
        ofn.lpstrInitialDir = nullptr;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
        
        std::string initialDir;
        if (!default_path.empty()) {
            initialDir = default_path.string();
            std::ranges::replace(initialDir, '/', '\\');
            ofn.lpstrInitialDir = initialDir.c_str();
        }
        
        if (GetOpenFileNameA(&ofn) == TRUE) {
            filepath = ofn.lpstrFile;
        } else {
            DWORD err = CommDlgExtendedError();
            std::cerr << err << '\n';
        }

        return filepath;
    }

    std::string FileDialog::win32_save_file(const std::string &default_filename,
                                            const std::vector<FileFilter> &filters, std::string &save_name_to,
                                            const std::filesystem::path &default_path) {
        std::string filepath;

        OPENFILENAMEA ofn;
        CHAR szFile[260] = {};

        if (!default_path.empty()) {
            std::string dir = default_path.string();
            std::replace(dir.begin(), dir.end(), '/', '\\');

            if (!default_filename.empty()) {
                std::string full = dir + "\\" + default_filename;
                strncpy(szFile, full.c_str(), sizeof(szFile) - 1);
            } else {
                dir += "\\";
                strncpy(szFile, dir.c_str(), sizeof(szFile) - 1);
            }
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
            for (const auto &[name, extensions]: filters) {
                filterStr += name + '\0' + extensions + '\0';

                // Set default extension from first filter if not already set
                if (defaultExt.empty() && !extensions.empty()) {
                    // Extract extension from the first extension in the list
                    size_t pos = extensions.find("*.");
                    if (pos != std::string::npos) {
                        size_t endPos = extensions.find(';', pos);
                        if (endPos == std::string::npos) {
                            endPos = extensions.size();
                        }
                        // Get extension without the "*."
                        defaultExt = extensions.substr(pos + 2, endPos - pos - 2);
                    }
                }
            }
        }

        // Create a non-const copy that will persist for the duration of the call
        std::vector filterBuf(filterStr.begin(), filterStr.end());
        filterBuf.push_back('\0'); // Add final null terminator

        ofn.lpstrFilter = filterBuf.data();
        ofn.nFilterIndex = 1;
        ofn.lpstrFileTitle = nullptr;
        ofn.lpstrInitialDir = nullptr;
        ofn.nMaxFileTitle = 0;
        ofn.Flags = OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

        // Set default extension if we found one
        std::vector extBuf(defaultExt.begin(), defaultExt.end());
        if (!defaultExt.empty()) {
            // Windows API requires a non-const char*
            extBuf.push_back('\0');
            ofn.lpstrDefExt = extBuf.data();
        }

        if (GetSaveFileNameA(&ofn) == TRUE) {
            filepath = ofn.lpstrFile;

            // Extract filename from full path and save to save_name_to
            size_t lastSlash = filepath.find_last_of("\\/");
            if (lastSlash != std::string::npos) {
                save_name_to = filepath.substr(lastSlash + 1);
            } else {
                save_name_to = filepath;
            }
        } else {
            DWORD err = CommDlgExtendedError();
            std::cerr << err << '\n';
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

        if (ImGui::BeginPopupModal("Open File", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
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

    std::string FileDialog::win32_select_folder(const std::string &title) {
        std::string folder_path;

        // Initialize COM (required for IFileDialog)
        HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
        if (FAILED(hr)) {
            return folder_path;
        }

        IFileDialog *pfd = nullptr;
        hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));

        if (SUCCEEDED(hr)) {
            // Get current options and add folder picker flag
            DWORD dwOptions;
            hr = pfd->GetOptions(&dwOptions);
            if (SUCCEEDED(hr)) {
                // FOS_PICKFOLDERS makes it a folder picker instead of file picker
                hr = pfd->SetOptions(dwOptions | FOS_PICKFOLDERS);
            }

            // Set the dialog title if provided
            if (SUCCEEDED(hr) && !title.empty()) {
                std::wstring wideTitle(title.begin(), title.end());
                pfd->SetTitle(wideTitle.c_str());
            }

            // Show the dialog
            if (SUCCEEDED(hr)) {
                hr = pfd->Show(GetActiveWindow());
            }

            // Get the result
            if (SUCCEEDED(hr)) {
                IShellItem *psi = nullptr;
                hr = pfd->GetResult(&psi);
                if (SUCCEEDED(hr)) {
                    PWSTR pszPath = nullptr;
                    hr = psi->GetDisplayName(SIGDN_FILESYSPATH, &pszPath);
                    if (SUCCEEDED(hr) && pszPath) {
                        // Convert wide string to narrow string
                        const int size = WideCharToMultiByte(CP_UTF8, 0, pszPath, -1, nullptr, 0, nullptr, nullptr);
                        if (size > 0) {
                            folder_path.resize(size - 1);
                            WideCharToMultiByte(CP_UTF8, 0, pszPath, -1, &folder_path[0], size, nullptr, nullptr);
                        }
                        CoTaskMemFree(pszPath);
                    }
                    psi->Release();
                }
            }
            pfd->Release();
        }

        CoUninitialize();
        return folder_path;
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

        if (ImGui::BeginPopupModal("Save File", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
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


    std::string FileDialog::open_file(const std::vector<FileFilter> &filters,
                                      const std::filesystem::path &default_path) {
#ifdef _WIN32
        return win32_open_file(filters, default_path);
#else
        return imgui_open_file(filters);
#endif
    }

    std::string FileDialog::select_folder(const std::string &title) {
#ifdef _WIN32
        return win32_select_folder(title);
#else
        return imgui_select_folder(title); // TODO: Filepicker for other platforms using imgui 
#endif
    }

    std::string FileDialog::save_file(std::string &save_name_to, const std::string &default_filename,
                                      const std::vector<FileFilter> &filters,
                                      const std::filesystem::path &default_path) {
#ifdef _WIN32
        return win32_save_file(default_filename, filters, save_name_to, default_path);
#else
        return imgui_save_file(default_filename, filters);
#endif
    }
}
