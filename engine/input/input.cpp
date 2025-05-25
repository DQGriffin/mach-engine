//
// Created by Dquavius Griffin on 5/22/25.
//

#include "input.h"

#include "../logging/logging.h"
#include "GLFW/glfw3.h"

mach::input::InputEngine::InputEngine(std::shared_ptr<mach::Window> window)
{
    this->window = window;
}

mach::input::InputEngine::~InputEngine()
{
}

void mach::input::InputEngine::registerCallback(Callback func)
{
}

void mach::input::InputEngine::processInput()
{
    for (int i = 32; i <= MACH_KEY_GRAVE_ACCENT; i++)
    {
        if (this->window->isKeyPressed(i))
        {
            this->handleKeyPressed(i);
        }
        else if (this->pressedKeys.contains(i))
        {
            this->handleKeyReleased(i);
        }
    }
}

void mach::input::InputEngine::bind(const char *name, int keyCode, int keyAction)
{
}

void mach::input::InputEngine::isActionPressed(const char *name)
{

}

void mach::input::InputEngine::handleKeyPressed(int keyCode)
{
    // Check if the key was already pressed during the last frame.
    // If so, do nothing
    bool isKeyAlreadyPressed = this->pressedKeys.contains(keyCode);
    if (isKeyAlreadyPressed) return;

    // Get the current time and add the key to the keypress map.
    // Later we'll use this value to calculate how long the key was pressed.
    auto now = std::chrono::high_resolution_clock::now();
    this->pressedKeys[keyCode] = now;
    LOG_DEBUG("Added key to key pressed map");
}

void mach::input::InputEngine::handleKeyReleased(int keyCode)
{
    auto now = std::chrono::high_resolution_clock::now();
    auto keyPressStartTime = this->pressedKeys[keyCode];
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - keyPressStartTime);
    this->pressedKeys.erase(keyCode);
    LOG_DEBUG("Key released. Removed key from key pressed map");
}
