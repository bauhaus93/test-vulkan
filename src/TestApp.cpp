#include "TestApp.h"

TestApp::TestApp(int width, int height):
    physicalDevice { VK_NULL_HANDLE },
    logicalDevice { VK_NULL_HANDLE } {
    CreateWindow(width, height);
    InitVulkan();
    //ListAvailableVulkanExtensions();
}

TestApp::~TestApp() {
    vkDestroyDevice(logicalDevice, nullptr);
    DestroyDebugReportCallbackEXT(instance, debugCallback, nullptr);
    vkDestroyInstance(instance, nullptr);
    glfwDestroyWindow(window);
    glfwTerminate();
}

void TestApp::Run() {
    Mainloop();
}

void TestApp::CreateWindow(int width, int height) {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(
        width,
        height,
        "Window",
        nullptr,
        nullptr
    );
}

void TestApp::InitVulkan() {

    if (enableValidationLayers && !CheckValidationLayerSupport()) {
        throw std::runtime_error("Requested validation layers not available");
    }
    CreateInstance();
    SetupDebugCallback();
    PickPhysicalDevice();

}

void TestApp::CreateInstance() {
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

void TestApp::PickPhysicalDevice() {
    auto devices = GetDeviceCandidates(instance);

    for (auto& device: devices) {
        if (device.QueuesComplete()) {
            if (device.CreateLogicalDevice()) {
                physicalDevice = device.GetPhysicalDevice();
                logicalDevice = device.GetLogicalDevice();
                break;
            }
            else {
                throw std::runtime_error("Could not create logical device");
            }
        }
    }
}

void TestApp::SetupDebugCallback() {
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

void TestApp::Mainloop() {
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }
}
