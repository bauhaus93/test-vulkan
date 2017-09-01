#pragma once

#include <stdexcept>
#include <vector>
#include <iostream>
#include <cstring>

#include <vulkan/vulkan.h>

#include "logging/StdLogger.h"

namespace engine::vulkan {

#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

extern const std::vector<const char*> validationLayers;

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
