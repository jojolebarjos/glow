
#include "Common.hpp"
#include "Scene.hpp"
#include "Smoke.hpp"
#include "Window.hpp"

int main(int argc, char** argv) {
    
    // Create window
    Window window;
    if (!window.initialize(1024, 768, false, true))
        return -1;
    
    // Create game
    Scene
    //Smoke
    game(&window);
    if (!game.initialize())
        return -1;
    
    // Game loop
    do {
        game.update();
    } while (window.update());
    return 0;
}
