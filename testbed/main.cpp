//
// Created by Dquavius Griffin on 5/18/25.
//

#include "../engine/archive/Archive.h"
#include "../engine/window/Window.h"
#include "../engine/math/vec.h"
#include "../engine/color/color.h"
#include "../engine/input/input.h"
#include "../engine/assets/AssetManager.h"
#include "../engine/logging/logging.h"

int main()
{
    mach::Window window = mach::Window(800, 600, "Hello World");
    mach::Color backgroundColor = mach::Color(0.0f, 0.3f, 0.3f);
    mach::input::InputEngine inputEngine = mach::input::InputEngine(std::make_shared<mach::Window>(window));

    mach::Window::registerMousePositionCallback(window, [&inputEngine](int xpos, int ypos) {
        inputEngine.windowMousePositionCallback(xpos, ypos);
    });

    inputEngine.registerMousePositionCallback([](int xpos, int ypos) -> bool {
        LOG_DEBUG(std::format("xpos:{}, ypos:{}", xpos, ypos));
        return true;
    });

    while(!window.shouldClose()) {
        inputEngine.processInput();

        // Set clear color and clear the buffer
        window.setClearColor(backgroundColor);
        window.clear();

        window.swapBuffers();
        window.pollEvents();
    }

    // mach::fs::Path path = mach::fs::Path("/Users/griffin/dependencies");
    // mach::archive::Archive::createArchive("/Users/griffin/test/archive_compressed.macha", path, true);

    // mach::fs::Path archivePath = mach::fs::Path("/Users/griffin/test/archive_compressed.macha");
    // mach::archive::Archive archive(archivePath);
    // archive.open();
    // archive.unpack("/Users/griffin/test/dependencies/");
}
