#include "../Engine/Application.h"
#include "../Engine/Core/Graphics/GL/glsl.h"
#include "../Engine/Core/Graphics/GL/Geometry/Cube.h"
#include "../Engine/Objects/Scene.h"

class Game
{
public:
    Game()
    {
        
    }

    ~Game()
    {
        delete cube_;
    }

    void init(DCraft::Application& app)
    {
        glutSetCursor(GLUT_CURSOR_NONE);
        glutWarpPointer(app.get_window_width() / 2, app.get_window_height() / 2);

        glDisable(GL_CULL_FACE);

        uint32_t shader_program = app.create_shader_program("vertexshader.vert", "fragmentshader.frag");
        app.set_shader_program(shader_program);
    }

    void setup(DCraft::SceneManager& sm, DCraft::WindowInfo window)
    {
        initial_scene = sm.create_scene("GameScene");

        initial_scene->set_position(0.0f, 0.0f, 0.0f);

        camera_ = initial_scene->create_camera<DCraft::PerspectiveCamera>("MainCamera", 70.0f, window.aspect_ratio , 0.1f, 100.0f);
        camera_->set_position(0.0f, 2.0f, 10.0f);
        initial_scene->add(camera_);
        initial_scene->set_active_camera(camera_);

        cube_ = new DCraft::Cube();
        cube_->set_position(0.0f, 1.0f, 0.0f);
        initial_scene->add(cube_);

        sm.set_active_scene(initial_scene);
    }

    void update(float dt)
    {
        cube_->update(dt);
    }
private:
    DCraft::PerspectiveCamera* camera_ = nullptr;
    DCraft::Cube* cube_ = nullptr;
    DCraft::Scene* initial_scene;

    void update_player(float dt)
    {
        
    }

    void update_enemies(float dt)
    {
        
    }

    void check_collisions(float dt)
    {
        
    }
};

int main(int argc, char** argv)
{
    DCraft::Application app(800, 600, "Final assigment :)");

    DCraft::ApplicationCallbacks application_callbacks;

    Game game;
    application_callbacks.setup = [&game](DCraft::SceneManager& sm, const DCraft::WindowInfo& info) { game.setup(sm, info); };
    application_callbacks.update = [&game](float dt) { game.update(dt); };
    application_callbacks.init = [&game](DCraft::Application& app) { game.init(app); };

    app.set_callbacks(application_callbacks);

    // Initialize and run
    app.initialize(argc, argv);
    game.init(app);
    app.run();

    return 0;
}

