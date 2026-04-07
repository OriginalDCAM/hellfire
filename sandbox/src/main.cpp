#include "RuntimePlugin.h"
#include "hellfire-core.h"
#include "hellfire/scene/CameraFactory.h"
#include "hellfire/graphics/Geometry/Cube.h"

#include "hellfire/EntryPoint.h"

namespace {
    class RuntimeConfig : public hellfire::IApplicationConfig {
    public:
        int get_window_width() override {
            return 1280;
        }

        int get_window_height() override {
            return 720;
        }

        const std::string get_title() const override {
            return "Sandbox Project";
        }

        void register_plugins(hellfire::Application &app) override {
            app.register_plugin(std::make_unique<RuntimePlugin>());
        }
    };
}

std::unique_ptr<hellfire::IApplicationConfig> create_application_config() {
        return std::make_unique<RuntimeConfig>();
}