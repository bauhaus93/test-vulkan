#pragma once

#include <string>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "logging/StdLogger.h"

namespace engine::glfw {

class GLFW {
public:
                            GLFW(int width_, int height_, const std::string& windowtitle);
                            ~GLFW();

    GLFWwindow*             GetWindow();
    void                    PollEvents();
    bool                    ShouldCloseWindow() const;

private:
    int                     width;
    int                     height;
    GLFWwindow*             window;


};

}
