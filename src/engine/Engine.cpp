#include "Engine.h"


namespace engine {

Engine::Engine(int width, int height):
    glfw { width, height, "Window Title" },
    vulkan { glfw.GetWindow() } {


}

void Engine::HandleEvents() {
    return glfw.PollEvents();
}

bool Engine::ShouldQuit() {
    return glfw.ShouldCloseWindow();
}

void Engine::DrawFrame() {
    vulkan.DrawFrame();
}


}
