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
    device { nullptr }{

    INFO("Initializing vulkan");
    if (enableValidationLayers && !CheckValidationLayerSupport()) {
        throw std::runtime_error("Requested validation layers not available");
    }
    CreateInstance();
    SetupDebugCallback();
    CreateSurface(window);
    ChooseDevice();
}

Vulkan::~Vulkan() {
    device = nullptr;
    vkDestroySurfaceKHR(instance, surface, nullptr);
    DestroyDebugReportCallbackEXT(instance, debugCallback, nullptr);
    vkDestroyInstance(instance, nullptr);
    INFO("Destroyed vulkan");
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

void Vulkan::ChooseDevice() {
    auto devices = GetDevices(instance, surface);

    for (auto& currDev: devices) {
        if (currDev.QueuesComplete() && currDev.SupportsRequiredExtensions()) {
            device = std::make_unique<Device>(std::move(currDev));
            device->LoadLogicalDevice();
            device->LoadSwapChain(surface);
            INFO(StringFormat("Used device: %s", device->GetName().c_str()));
            break;
        }
    }
    if (device == nullptr) {
        throw std::runtime_error("Could not find appropriate device");
    }
}





}
