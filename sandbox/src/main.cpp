#include "GamePlugin.h"
#include "hellfire-core.h"
#include "hellfire/scene/CameraFactory.h"
#include "hellfire/graphics/Geometry/Cube.h"

int main() {
    // Create the main application window
    // Parameters: width (pixels), height (pixels), window title
    hellfire::Application app(800, 600, "Custom Engine - Hellfire");

    // Register our HelloCube plugin with the application
    // The plugin system allows us to extend the engine's functionality
    app.register_plugin(std::make_unique<GamePlugin>());

    // Initialize all engine systems (graphics, input, etc.)
    app.initialize();

    // Start the main game loop - this runs until the user closes the window
    // The loop handles: input processing, scene updates, and rendering
    app.run();

    std::clog << "Terminating the application" << std::endl;
}
