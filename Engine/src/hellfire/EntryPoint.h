//
// Created by denzel on 22/10/2025.
//

#pragma once

#include "hellfire/core/Application.h"

namespace hellfire {
    /**
     *  @brief Application configuration interface
     *
     *  Application using the Hellfire engine should implement this interface
     *  to configure their application instance.
     */

    class IApplicationConfig {
    public:
        virtual ~IApplicationConfig() = default;

        /**
         * @brief Get the window width
         */
        virtual int get_window_width() = 0;

        /**
         * @brief Get the window height
         */
        virtual int get_window_height() = 0;

        /**
         * @brief Get the application title
         */
        virtual const std::string &get_title() const = 0;

        /**
         * @brief Register application-specific plugins
         * @param app The application instance to register plugins with
         */
        virtual void register_plugins(Application &app) = 0;
    };
} // namespace hellfire

// Forward declaration - must be implemented by the application
extern std::unique_ptr<hellfire::IApplicationConfig> create_application_config();

#ifndef HELLFIRE_ENTRYPOINT_DEFINED
#define HELLFIRE_ENTRYPOINT_DEFINED

int main() {
    const auto config = create_application_config();

    hellfire::Application app(config->get_window_width(),
                              config->get_window_height(),
                              config->get_title());

    config->register_plugins(app);

    app.initialize();
    app.run();

    return 0;
}

#endif // HELLFIRE_ENTRYPOINT_DEFINED
