#include "GLFW.h"

namespace engine::glfw {

GLFW::GLFW(int width_, int height_, const std::string& windowTitle):
    width { width_ },
    height { height_ } {

    INFO("Initializing glfw");

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    window = glfwCreateWindow(width, height, windowTitle.c_str(), nullptr, nullptr);
}

GLFW::~GLFW() {
    INFO("Destroying glfw");
    glfwDestroyWindow(window);
    glfwTerminate();
}

GLFWwindow* GLFW::GetWindow() {
    return window;
}

bool GLFW::ShouldCloseWindow() const {
    return glfwWindowShouldClose(window);
}

void GLFW::PollEvents() {
    glfwPollEvents();
}

}
