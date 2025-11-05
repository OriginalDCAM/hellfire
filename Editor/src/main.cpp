//
// Created by denzel on 17/09/2025.
//

#include "hellfire/core/Application.h"

#ifdef HELLFIRE_EDITOR_ENABLED
#include "CoreEditorPlugin.h"
#endif

int main() {
    hellfire::Application app(800, 600, "Hellfire Editor");

#ifdef HELLFIRE_EDITOR_ENABLED
    // Register editor plugin
    app.register_plugin(std::make_unique<hellfire::editor::CoreEditorPlugin>());
#endif

    app.initialize();
    app.run();

    return 0;
}