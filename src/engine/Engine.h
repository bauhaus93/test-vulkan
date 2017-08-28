#pragma once

#include "glfw/GLFW.h"
#include "vulkan/Vulkan.h"

namespace engine {

class Engine {

public:

                        Engine(int width, int height);

    void                HandleEvents();
    bool                ShouldQuit();


private:
    glfw::GLFW          glfw;
    vulkan::Vulkan      vulkan;


};

}
