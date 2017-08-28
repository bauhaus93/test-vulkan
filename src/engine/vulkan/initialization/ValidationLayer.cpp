#include "ValidationLayer.h"

namespace engine::vulkan {

bool CheckValidationLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName: validationLayers) {
        bool layerFound = false;

        for (const auto& lp: availableLayers) {
            if (strcmp(layerName, lp.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return false;
        }
    }

    return true;
}

VkResult CreateDebugReportCallbackEXT(
    VkInstance instance,
    const VkDebugReportCallbackCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugReportCallbackEXT* pDebugCallback) {

    auto func = (PFN_vkCreateDebugReportCallbackEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");

    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugCallback);
    }
    else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void DestroyDebugReportCallbackEXT(
    VkInstance instance,
    VkDebugReportCallbackEXT callback,
    const VkAllocationCallbacks* pAllocator) {

    auto func = (PFN_vkDestroyDebugReportCallbackEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
    if (func != nullptr) {
        func(instance, callback, pAllocator);
    }
}

}
