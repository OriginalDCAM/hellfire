


#include "DCraft/Application.h"
#include "Game.h"

int main(int argc, char **argv) {
    DCraft::Application app(800, 600, "Final assignment :)");
    Game game;

    // Hook up the application callbacks into the game class
    game.setup_callbacks(app);

    // Initialize and run
    app.initialize(argc, argv);
    app.run();

    std::clog << "Terminating the application" << std::endl;

    return 0;
}
