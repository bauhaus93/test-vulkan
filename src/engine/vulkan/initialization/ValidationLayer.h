#pragma once

#include <stdexcept>
#include <vector>
#include <iostream>
#include <cstring>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "logging/StdLogger.h"

namespace engine::vulkan {

#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

const std::vector<const char*> validationLayers = {
    "VK_LAYER_LUNARG_standard_validation"
};

bool CheckValidationLayerSupport();

VkResult CreateDebugReportCallbackEXT(
    VkInstance instance,
    const VkDebugReportCallbackCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugReportCallbackEXT* pDebugCallback);

void DestroyDebugReportCallbackEXT(
    VkInstance instance,
    VkDebugReportCallbackEXT callback,
    const VkAllocationCallbacks* pAllocator);

}
