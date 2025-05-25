//
// Created by Dquavius Griffin on 5/18/25.
//

#include "../engine/window/Window.h"
#include "../engine/math/vec.h"
#include "../engine/color/color.h"
#include "../engine/input/input.h"
#include "../engine/assets/AssetManager.h"

int main()
{
    mach::Window window = mach::Window(800, 600, "Hello World");
    mach::Color backgroundColor = mach::Color(0.0f, 0.3f, 0.3f);
    mach::input::InputEngine inputEngine = mach::input::InputEngine(std::make_shared<mach::Window>(window));

    while(!window.shouldClose()) {
        inputEngine.processInput();

        // Set clear color and clear the buffer
        window.setClearColor(backgroundColor);
        window.clear();

        window.swapBuffers();
        window.pollEvents();
    }
}
