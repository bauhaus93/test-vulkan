#pragma once

#include <vector>
#include <memory>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "logging/StdLogger.h"
#include "utility/StringFormat.h"
#include "initialization/ValidationLayer.h"
#include "initialization/Extension.h"

#include "Device.h"
#include "Pipeline.h"

namespace engine::vulkan {

class Vulkan {

public:
    explicit                    Vulkan(GLFWwindow* window);
                                ~Vulkan();

private:

    void                        LoadInstance();
    void                        SetupDebugCallback();
    void                        LoadSurface(GLFWwindow* window);
    void                        LoadDevice();
    void                        LoadSwapChain();
    void                        LoadPipeline();

    VkInstance                  instance;
    VkDebugReportCallbackEXT    debugCallback;
    VkSurfaceKHR                surface;

    std::unique_ptr<Device>     device;
    std::unique_ptr<SwapChain>  swapchain;
    std::unique_ptr<Pipeline>   pipeline;
};


}
