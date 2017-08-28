#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


#include "logging/StdLogger.h"
#include "utility/StringFormat.h"
#include "initialization/ValidationLayer.h"
#include "initialization/DeviceCandidate.h"
#include "initialization/Extension.h"
#include "initialization/SwapChain.h"

namespace engine::vulkan {

class Vulkan {

public:
    explicit                    Vulkan(GLFWwindow* window);
                                ~Vulkan();

private:

    void                        CreateInstance();
    void                        SetupDebugCallback();
    void                        CreateSurface(GLFWwindow* window);
    void                        PickPhysicalDevice();

    VkInstance                  instance;
    VkDebugReportCallbackEXT    debugCallback;
    VkSurfaceKHR                surface;
    VkPhysicalDevice            physicalDevice;
    VkDevice                    logicalDevice;
    VkQueue                     graphicsQueue;






};


}
