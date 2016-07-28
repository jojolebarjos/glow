
#include "Common.hpp"
#include "Scene.hpp"
#include "Smoke.hpp"
#include "Window.hpp"

int main(int argc, char** argv) {
    
    // Create window
    Window window;
    if (!window.initialize(640, 480))
        return -1;
    
    // Create game
    Scene
    //Smoke
    game(&window);
    game.initialize();
    
    // Game loop
    do {
        game.update();
    } while (window.update());
    return 0;
}
