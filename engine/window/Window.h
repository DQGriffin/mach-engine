//
// Created by Dquavius Griffin on 5/21/25.
//

#ifndef WINDOW_H
#define WINDOW_H

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
    public:
        Window(int width, int height, const char* title);
        ~Window();
        void setClearColor(float r, float g, float b, float a);
        void setClearColor(mach::Color color);
        void setSize(int width, int height);
        void setSize(mach::ivec2 size);
        void maximize();
        mach::ivec2 getSize() const;
        bool shouldClose();
        void clear();
        void swapBuffers();
        void pollEvents();
        void close();
        bool isKeyPressed(int key);
        void captureMouse();
    private:
        // void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    };
}


#endif //WINDOW_H
