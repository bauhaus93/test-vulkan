#pragma once

#include <vector>
#include <memory>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "logging/StdLogger.h"
#include "utility/StringFormat.h"
#include "initialization/ValidationLayer.h"
#include "initialization/Extension.h"

#include "Device.h"

namespace engine::vulkan {

class Vulkan {

public:
    explicit                    Vulkan(GLFWwindow* window);
                                ~Vulkan();

private:

    void                        CreateInstance();
    void                        SetupDebugCallback();
    void                        CreateSurface(GLFWwindow* window);
    void                        ChooseDevice();

    VkInstance                  instance;
    VkDebugReportCallbackEXT    debugCallback;
    VkSurfaceKHR                surface;

    std::unique_ptr<Device>     device;

};


}
