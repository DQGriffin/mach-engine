//
// Created by Dquavius Griffin on 5/22/25.
//

#ifndef INPUT_H
#define INPUT_H

#include <chrono>
#include <functional>
#include <memory>

#include "keys.h"
#include <unordered_map>

#include "../window/Window.h"

namespace mach::input
{
    class InputEngine {
    private:
        using Callback = std::function<void(int keycode, int keyPressDuration)>;
        std::unordered_map<int, std::chrono::time_point<std::chrono::steady_clock>> pressedKeys;
        std::unordered_map<const char*, int> bindings;
        std::shared_ptr<mach::Window> window;

    public:
        InputEngine(std::shared_ptr<mach::Window> window);
        ~InputEngine();
        void registerCallback(Callback func);
        void processInput();
        void bind(const char* name, int keyCode, int keyAction);
        void isActionPressed(const char* name);

    private:
        void handleKeyPressed(int keyCode);
        void handleKeyReleased(int keyCode);
    };
}


#endif //INPUT_H
