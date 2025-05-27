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
    using MousePositionCallback = std::function<bool(int xpos, int ypos)>;
    using KeyPressCallback = std::function<bool(int keycode, int keyPressDuration)>;
    enum class CallbackPriority
    {
        EngineDebug = 0,
        EditorUI = 1,
        RuntimeUI = 2,
        Scripting = 3,
        Gameplay = 4,
        Cutscene = 5,
    };

    enum class KeyEventTrigger
    {
        Pressed = 0,
        Released = 1,
        Repeat = 2,
    };

    inline std::string callbackPriorityToString(CallbackPriority priority)
    {
        switch (priority)
        {
            case CallbackPriority::EngineDebug: return "EngineDebug";
            case CallbackPriority::EditorUI: return "EditorUI";
            case CallbackPriority::RuntimeUI: return "RuntimeUI";
            case CallbackPriority::Scripting: return "Scripting";
            case CallbackPriority::Gameplay: return "Gameplay";
            case CallbackPriority::Cutscene: return "Cutscene";
            default: return "Unknown";
        }
    }

    struct MousePositionCallbackEntry
    {
        MousePositionCallback callback;
        CallbackPriority priority;
    };

    struct KeyPressCallbackEntry
    {
        KeyPressCallback callback;
        CallbackPriority priority;
        KeyEventTrigger type;
    };

    class InputEngine {
    private:

        std::unordered_map<int, std::chrono::time_point<std::chrono::steady_clock>> pressedKeys;
        std::unordered_map<const char*, int> bindings;
        std::shared_ptr<mach::Window> window;
        std::vector<MousePositionCallbackEntry> mousePositionCallbacks;
        std::vector<KeyPressCallbackEntry> keyPressCallbacks;

    public:
        InputEngine(std::shared_ptr<mach::Window> window);
        ~InputEngine();
        void registerKeyPressCallback(KeyPressCallback func, CallbackPriority priority = CallbackPriority::Gameplay, KeyEventTrigger type = KeyEventTrigger::Pressed);
        void registerMousePositionCallback(MousePositionCallback func, CallbackPriority priority = CallbackPriority::Gameplay);
        void windowMousePositionCallback(int xpos, int ypos);
        void processInput();
        void bind(const char* name, int keyCode, int keyAction);
        void isActionPressed(const char* name);


    private:
        void handleKeyPressed(int keyCode);
        void handleKeyReleased(int keyCode);
        void registerInternalMousePositionCallback();
        void onMouseMoved(double xpos, double ypos);
        void notifyKeyPressSubscribers(int key, KeyEventTrigger trigger);
    };
}


#endif //INPUT_H
