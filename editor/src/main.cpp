//
// Created by denzel on 17/09/2025.
//

#include "EditorApplication.h"
#include "hellfire/EntryPoint.h"

namespace {
    class EditorConfig : public hellfire::IApplicationConfig {
    public:
        int get_window_width() override {
            return 1280;
        }

        int get_window_height() override {
            return 720;
        }

        const std::string get_title() const override {
            return "Hellfire Editor";
        }

        void register_plugins(hellfire::Application &app) override {
#ifdef HELLFIRE_EDITOR_ENABLED
            app.register_plugin(std::make_unique<hellfire::editor::EditorApplication>());
#endif
        }
    };
}

// Engine will call this to get application configuration
std::unique_ptr<hellfire::IApplicationConfig> create_application_config() {
    return std::make_unique<EditorConfig>();
}