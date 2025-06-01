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

void mach::input::InputEngine::registerKeyPressCallback(KeyPressCallback func, CallbackPriority priority, KeyEventTrigger type)
{
    KeyPressCallbackEntry entry{func, priority, type};
    auto it = std::lower_bound(
        keyPressCallbacks.begin(),
        keyPressCallbacks.end(),
        entry,
        [](const KeyPressCallbackEntry& e1, const KeyPressCallbackEntry& e2) {
            return static_cast<int>(e1.priority) < static_cast<int>(e2.priority);
        }
    );

    this->keyPressCallbacks.insert(it, std::move(entry));
}

void mach::input::InputEngine::registerMousePositionCallback(MousePositionCallback func, CallbackPriority priority)
{
    MousePositionCallbackEntry entry{func, priority};
    auto it = std::lower_bound(
        mousePositionCallbacks.begin(),
        mousePositionCallbacks.end(),
        entry,
        [](const MousePositionCallbackEntry& e1, const MousePositionCallbackEntry& e2) {
            return static_cast<int>(e1.priority) < static_cast<int>(e2.priority);
        }
    );

    this->mousePositionCallbacks.insert(it, std::move(entry));
}

void mach::input::InputEngine::windowMousePositionCallback(int xpos, int ypos)
{
    for (const auto& entry : this->mousePositionCallbacks)
    {
        // callback returns true if it wants to stop propogation
        if (entry.callback(xpos, ypos))
        {
            LOG_INFO(std::format("Mouse position event consumed by {} layer callback", callbackPriorityToString(entry.priority)));
            break;
        }
    }
}

void mach::input::InputEngine::processInput()
{
    for (int i = 32; i <= MACH_KEY_GRAVE_ACCENT; i++)
    {
        if (this->window->isKeyPressed(i))
        {
            this->handleKeyPressed(i);
            this->notifyKeyPressSubscribers(i, KeyEventTrigger::Pressed);
        }
        else if (this->pressedKeys.contains(i))
        {
            this->handleKeyReleased(i);
            this->notifyKeyPressSubscribers(i, KeyEventTrigger::Released);
        }
    }

    for (int i = MACH_KEY_WORLD_1; i <= MACH_KEY_MENU; i++)
    {
        if (this->window->isKeyPressed(i))
        {
            this->handleKeyPressed(i);
            this->notifyKeyPressSubscribers(i, KeyEventTrigger::Pressed);
        }
        else if (this->pressedKeys.contains(i))
        {
            this->handleKeyReleased(i);
            this->notifyKeyPressSubscribers(i, KeyEventTrigger::Released);
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
    // LOG_DEBUG("Added key to key pressed map");
}

void mach::input::InputEngine::handleKeyReleased(int keyCode)
{
    auto now = std::chrono::high_resolution_clock::now();
    auto keyPressStartTime = this->pressedKeys[keyCode];
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - keyPressStartTime);
    this->pressedKeys.erase(keyCode);
    // LOG_DEBUG("Key released. Removed key from key pressed map");
}

void mach::input::InputEngine::registerInternalMousePositionCallback()
{

}

void mach::input::InputEngine::onMouseMoved(double xpos, double ypos)
{
    LOG_DEBUG("On mouse moved");
}

void mach::input::InputEngine::notifyKeyPressSubscribers(int key, KeyEventTrigger trigger)
{
    for (const auto& entry : this->keyPressCallbacks)
    {
        if (entry.type == trigger)
        {
            // callback returns true if it wants to stop propagation
            if (entry.callback(key, 0))
            {
                LOG_DEBUG(std::format("Key press event consumed by {} layer callback", callbackPriorityToString(entry.priority)));
                break;
            }
        }
    }
}
