#include "Vulkan.h"

namespace engine::vulkan {

VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
    VkDebugReportFlagsEXT flags,
    VkDebugReportObjectTypeEXT objType,
    uint64_t obj,
    size_t location,
    int32_t code,
    const char* layerPrefix,
    const char* msg,
    void* userData) {

    WARN(msg);

    return VK_FALSE;
}

Vulkan::Vulkan(GLFWwindow* window):
    physicalDevice { VK_NULL_HANDLE },
    logicalDevice { VK_NULL_HANDLE } {

    INFO("Initializing vulkan");
    if (enableValidationLayers && !CheckValidationLayerSupport()) {
        throw std::runtime_error("Requested validation layers not available");
    }
    CreateInstance();
    SetupDebugCallback();
    CreateSurface(window);
    PickPhysicalDevice();
}

Vulkan::~Vulkan() {
    INFO("Destroying vulkan");
    vkDestroyDevice(logicalDevice, nullptr);
    vkDestroySurfaceKHR(instance, surface, nullptr);
    DestroyDebugReportCallbackEXT(instance, debugCallback, nullptr);
    vkDestroyInstance(instance, nullptr);
}

void Vulkan::CreateInstance() {
    VkApplicationInfo appInfo {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Test-Vulkan";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "roflcopter";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else {
        createInfo.enabledLayerCount = 0;
    }

    auto extensions = GetRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
        throw std::runtime_error("Could not create vulkan instance");
    }
}

void Vulkan::SetupDebugCallback() {
    if (enableValidationLayers) {
        VkDebugReportCallbackCreateInfoEXT createInfo {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
        createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT |
                       VK_DEBUG_REPORT_WARNING_BIT_EXT;
        createInfo.pfnCallback = DebugCallback;

        if (CreateDebugReportCallbackEXT(instance, &createInfo, nullptr, &debugCallback) != VK_SUCCESS) {
            throw std::runtime_error("Could not set up debug callback");
        }
    }
}

void Vulkan::CreateSurface(GLFWwindow* window) {
    if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("Could not create window surface");
    }
}

void Vulkan::PickPhysicalDevice() {
    auto devices = GetDeviceCandidates(instance, surface);

    for (auto& device: devices) {
        if (device.QueuesComplete() && device.SupportsRequiredExtensions()) {
            if (device.CreateLogicalDevice()) {
                physicalDevice = device.GetPhysicalDevice();
                logicalDevice = device.GetLogicalDevice();
                graphicsQueue = device.GetGraphicsQueue();
                INFO(StringFormat("Used device: %s", device.GetName().c_str()));
                break;
            }
            else {
                throw std::runtime_error("Could not create logical device");
            }
        }
    }
    if (physicalDevice == VK_NULL_HANDLE) {
        throw std::runtime_error("Could not find appropriate device");
    }
}

void Vulkan::CreateSwapChain() {
    
}


}
