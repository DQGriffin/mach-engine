//
// Created by Dquavius Griffin on 5/21/25.
//

#include "Window.h"

#include "../input/input.h"
#include "../logging/logging.h"

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

mach::Window::Window(int width, int height, const char *title)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    this->window = glfwCreateWindow(800, 600, title, NULL, NULL);
    if (window == NULL) {
        glfwTerminate();
        LOG_FATAL("Failed to create GLFW window");
    }
    glfwMakeContextCurrent(this->window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        glfwTerminate();
        LOG_FATAL("Failed to initialize GLAD");
    }

    glViewport(0, 0, 800, 600);

    glfwSetFramebufferSizeCallback(this->window, framebuffer_size_callback);

    glfwSetWindowUserPointer(window, this);
    glfwSetCursorPosCallback(this->window, Window::mouseCallback);

#ifdef __APPLE__
    // for retina display the framebuffer size is larger than the window size
    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    framebuffer_size_callback(this->window, fbWidth, fbHeight);
#endif
}

mach::Window::~Window()
{
    glfwTerminate();
    glfwDestroyWindow(this->window);
}

void mach::Window::setClearColor(float r, float g, float b, float a)
{
    glClearColor(r, g, b, a);
}

void mach::Window::setClearColor(mach::Color color)
{
    glClearColor(color.r, color.g, color.b, 1.0f);
}

void mach::Window::setSize(int width, int height)
{
    glfwSetWindowSize(this->window, width, height);
}

void mach::Window::setSize(mach::ivec2 size)
{
    glfwSetWindowSize(this->window, size.x, size.y);
}

void mach::Window::maximize()
{
    glfwMaximizeWindow(this->window);
}

mach::ivec2 mach::Window::getSize() const
{
    mach::ivec2 size;
    glfwGetWindowSize(this->window, &size.x, &size.y);
    return size;
}

bool mach::Window::shouldClose()
{
    return glfwWindowShouldClose(this->window);
}

void mach::Window::clear()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void mach::Window::swapBuffers()
{
    glfwSwapBuffers(this->window);
}

void mach::Window::pollEvents()
{
    glfwPollEvents();
}

void mach::Window::close()
{
    glfwDestroyWindow(this->window);
    glfwTerminate();
}

bool mach::Window::isKeyPressed(int key)
{
    return glfwGetKey(this->window, key) == GLFW_PRESS;
}

void mach::Window::captureMouse()
{
    glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void mach::Window::onMouseMoved(int x, int y)
{
    this->mousePositionCallback(x, y);
}

void mach::Window::registerMousePositionCallback(Window &window, MousePositionCallback callback)
{
    window.mousePositionCallback = callback;
}
