//
// Created by Dquavius Griffin on 5/21/25.
//

#ifndef WINDOW_H
#define WINDOW_H
#define STB_IMAGE_IMPLEMENTATION

#include <functional>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc//matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../color/color.h"
#include "../math/vec.h"

namespace mach
{
    class Window {
    private:
        GLFWwindow* window;
        using MousePositionCallback = std::function<void(int xpos, int ypos)>;
        MousePositionCallback mousePositionCallback;
        bool isMouseCaptured = false;

    public:
        Window(int width, int height, const char* title);
        ~Window();
        void setClearColor(float r, float g, float b, float a);
        void setClearColor(mach::Color color);
        void setSize(int width, int height);
        void setSize(mach::ivec2 size);
        void setVSync(bool enabled);
        void maximize();
        mach::ivec2 getSize() const;
        bool shouldClose();
        void clear();
        void swapBuffers();
        void pollEvents();
        void close();
        bool isKeyPressed(int key) const;
        void setMouseCaptured(bool capture);
        void captureMouse();
        void releaseMouse();
        void onMouseMoved(int x, int y);
        bool getMouseCaptureState() const;
        static void registerMousePositionCallback(Window& window, MousePositionCallback callback);
    private:
        // void framebuffer_size_callback(GLFWwindow* window, int width, int height);
        static void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
            // Get 'this' pointer
            auto* handler = static_cast<Window*>(glfwGetWindowUserPointer(window));
            if (handler) {
                handler->onMouseMoved(xpos, ypos);
            }
        }
    };
}


#endif //WINDOW_H
