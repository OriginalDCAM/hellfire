#include "DCraft/Application.h"
#include "Game.h"

int main() {
    hellfire::Application app(800, 600, "Custom Engine - Hellfire");
    Game game;

    // Hook up the application callbacks into the game class
    game.setup_callbacks(app);

    // Initialize and run
    app.initialize();
    app.run();

    std::clog << "Terminating the application" << std::endl;

    return 0;
}
