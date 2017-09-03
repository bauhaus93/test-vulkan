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
    instance { VK_NULL_HANDLE },
    surface { VK_NULL_HANDLE },
    imgAvailableSem { VK_NULL_HANDLE },
    renderFinishedSem { VK_NULL_HANDLE },
    device { nullptr },
    pipeline { nullptr }{

    INFO("Initializing vulkan");
    if (enableValidationLayers && !CheckValidationLayerSupport()) {
        throw std::runtime_error("Requested validation layers not available");
    }
    LoadInstance();
    SetupDebugCallback();
    LoadSurface(window);
    LoadDevice();
    LoadSwapChain();
    LoadPipeline();
    LoadFramebuffers();
    LoadCommandPools();
    LoadSemaphores();
}

Vulkan::~Vulkan() {
    vkDeviceWaitIdle(device->GetLogicalDevice());

    vkDestroySemaphore(device->GetLogicalDevice(), renderFinishedSem, nullptr);
    vkDestroySemaphore(device->GetLogicalDevice(), imgAvailableSem, nullptr);
    commandPool = nullptr;
    pipeline = nullptr;
    swapchain = nullptr;
    device = nullptr;
    vkDestroySurfaceKHR(instance, surface, nullptr);
    DestroyDebugReportCallbackEXT(instance, debugCallback, nullptr);
    vkDestroyInstance(instance, nullptr);
    INFO("Destroyed vulkan");
}

void Vulkan::DrawFrame() {
    vkQueueWaitIdle(device->GetPresentQueue().GetQueue());

    uint32_t imgIndex;
    vkAcquireNextImageKHR(device->GetLogicalDevice(),
        swapchain->GetSwapChain(),
        std::numeric_limits<uint64_t>::max(),
        imgAvailableSem,
        VK_NULL_HANDLE,
        &imgIndex);

    VkSubmitInfo submitInfo {};

    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    VkSemaphore waitSemaphores[] = { imgAvailableSem };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = commandPool->GetCommandBufferPtr(imgIndex);

    VkSemaphore signalSemaphores[] = { renderFinishedSem };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(device->GetGraphicsQueue().GetQueue(), 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
        throw std::runtime_error("Could not submit draw command");
    }

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { swapchain->GetSwapChain() };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imgIndex;
    presentInfo.pResults = nullptr;

    vkQueuePresentKHR(device->GetPresentQueue().GetQueue(), &presentInfo);
}

void Vulkan::LoadInstance() {
    DEBUG("Load instance");
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
    DEBUG("Setup debug callback");
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

void Vulkan::LoadSurface(GLFWwindow* window) {
    DEBUG("Load surface");
    if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("Could not create window surface");
    }
}

void Vulkan::LoadDevice() {
    DEBUG("Load device");
    auto devices = GetDevices(instance, surface);

    for (auto& currDev: devices) {
        if (currDev.QueuesComplete() && currDev.SupportsRequiredExtensions()) {
            device = std::make_unique<Device>(std::move(currDev));
            device->LoadLogicalDevice();
            INFO(StringFormat("Used device: %s", device->GetName().c_str()));
            break;
        }
    }
    if (device == nullptr) {
        throw std::runtime_error("Could not find appropriate device");
    }
}

void Vulkan::LoadSwapChain() {
    DEBUG("Load swapchain");
    swapchain = device->CreateSwapChain(surface);
}

void Vulkan::LoadPipeline() {
    DEBUG("Load pipeline");
    pipeline = std::make_unique<Pipeline>(device->GetLogicalDevice(),
        swapchain->GetImageExtent(),
        swapchain->GetImageFormat());
}

void Vulkan::LoadFramebuffers() {
    DEBUG("Load framebuffers");
    swapchain->LoadFramebuffers(pipeline->GetRenderPass());
}

void Vulkan::LoadCommandPools() {
    DEBUG("Load command pools");
    commandPool = std::make_unique<CommandPool>(device->GetLogicalDevice(), device->GetGraphicsQueue());
    commandPool->LoadCommandBuffers(*swapchain);
    commandPool->RecordCommand(*swapchain, *pipeline);
}

void Vulkan::LoadSemaphores() {
    DEBUG("Load semaphores");
    VkSemaphoreCreateInfo createInfo {};

    createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    if (vkCreateSemaphore(device->GetLogicalDevice(), &createInfo, nullptr, &imgAvailableSem) != VK_SUCCESS) {
        throw std::runtime_error("Could not create img available semaphore");
    }

    if (vkCreateSemaphore(device->GetLogicalDevice(), &createInfo, nullptr, &renderFinishedSem) != VK_SUCCESS) {
        throw std::runtime_error("Could not create render finished semaphore");
    }
}


}
