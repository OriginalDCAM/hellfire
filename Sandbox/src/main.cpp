#include "GamePlugin.h"
#include "hellfire-core.h"
#include "hellfire/scene/CameraFactory.h"
#include "hellfire/graphics/Geometry/Cube.h"

/**
 * HelloCubePlugin - A simple plugin that creates a basic 3D scene with a cube
 * 
 * This demonstrates the fundamental concepts of the Hellfire engine:
 * - Creating 3D objects (entities)
 * - Positioning objects in 3D space
 * - Setting up a camera to view the scene
 * - Managing scenes
 */
class HelloCubePlugin : public hellfire::IApplicationPlugin {
public:
    void on_initialize(hellfire::Application &app) override {
        // Create a new 3D scene - this will contain all our objects
        auto scene = new hellfire::Scene();

        // Create a cube with a friendly name for debugging
        auto cube = hellfire::Cube::create("My very first cube");

        // Position the cube in 3D space (x, y, z coordinates)
        // x=0: centered horizontally
        // y=-0: centered vertically
        // z=-5: 5 units away from camera (into the screen)
        cube->transform()->set_position(0, 0, -5);

        // Add the cube to our scene so it will be rendered
        scene->add_entity(cube);

        // Create a perspective camera to view our 3D scene
        // This simulates how human eyes see the world with depth perception
        const auto camera = hellfire::PerspectiveCamera::create();
        scene->add_entity(camera);

        // Tell the scene which camera to use for rendering
        scene->set_active_camera(camera);

        // Get the engine's scene manager and make our scene active
        // The scene manager handles which scene is currently being displayed
        const auto sm = hellfire::ServiceLocator::get_service<hellfire::SceneManager>();
        sm->set_active_scene(scene);
    }
};

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

    return 0;
}
