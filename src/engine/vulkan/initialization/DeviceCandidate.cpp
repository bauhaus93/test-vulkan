#include "DeviceCandidate.h"

namespace engine::vulkan {

std::vector<DeviceCandidate> GetDeviceCandidates(VkInstance instance, VkSurfaceKHR surface) {
    uint32_t deviceCount = 0;
    std::vector<VkPhysicalDevice> devices;
    std::vector<DeviceCandidate> wrappedDevices;

    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (deviceCount > 0) {
      devices.resize(deviceCount);
      vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
    }
    for (auto device: devices) {
        wrappedDevices.emplace_back(device, surface);
    }
    return wrappedDevices;
}

DeviceCandidate::DeviceCandidate(VkPhysicalDevice physicalDevice_, VkSurfaceKHR surface_):
    physicalDevice { physicalDevice_ },
    logicalDevice { VK_NULL_HANDLE },
    surface { surface_ },
    swapchain { VK_NULL_HANDLE },
    graphicsIndex { -1 },
    presentIndex { -1 } {
    vkGetPhysicalDeviceFeatures(physicalDevice, &features);
    vkGetPhysicalDeviceProperties(physicalDevice, &properties);
    LoadQueueFamilies();
    LoadSwapchainDetails();
}

void DeviceCandidate::LoadQueueFamilies() {
    uint32_t queueFamilyCount = 0;
    std::vector<VkQueueFamilyProperties> queueFamilies;

    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
    queueFamilies.resize(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto& qf: queueFamilies) {
        if (qf.queueCount > 0) {
            if (qf.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                graphicsIndex = i;
            }
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);
            if (presentSupport) {
                presentIndex = i;
            }
        }
        i++;
    }
}

void DeviceCandidate::LoadSwapchainDetails() {
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
    if (formatCount != 0) {
        surfaceFormats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, surfaceFormats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);
    if (presentModeCount != 0) {
        presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes.data());
    }
}

void DeviceCandidate::CreateSwapchain() {
    VkSurfaceFormatKHR surfaceFmt { ChooseSwapSurfaceFormat(surfaceFormats) };
    VkPresentModeKHR presentMode { ChooseSwapPresentMode(presentModes) };
    VkExtent2D extent { ChooseSwapExtent(surfaceCapabilities, 800, 600) };

    uint32_t imageCount = surfaceCapabilities.minImageCount + 1;
    if (surfaceCapabilities.maxImageCount > 0 && imageCount > surfaceCapabilities.maxImageCount) {
        imageCount = surfaceCapabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFmt.format;
    createInfo.imageColorSpace = surfaceFmt.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    uint32_t familyIndices[] = {
        static_cast<uint32_t>(graphicsIndex),
        static_cast<uint32_t>(presentIndex) };

    if (graphicsIndex != presentIndex) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = familyIndices;
    }
    else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
    }

    createInfo.preTransform = surfaceCapabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(logicalDevice, &createInfo, nullptr, &swapchain) != VK_SUCCESS) {
        throw std::runtime_error("Could not create swapchain");
    }
    INFO("Created swapchain");
}


void DeviceCandidate::CreateLogicalDevice() {
    std::vector<VkDeviceQueueCreateInfo> dqCreateInfo;
    VkDeviceCreateInfo createInfo {};
    VkPhysicalDeviceFeatures requestedFeatures { VK_FALSE };
    std::set<int> uniqueQueueFamilies = { graphicsIndex, presentIndex };
    float queuePriority = 1.0f;

    for (int qf: uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo dq {};
        dq.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        dq.queueFamilyIndex = qf;
        dq.queueCount = 1;
        dq.pQueuePriorities = &queuePriority;
        dqCreateInfo.push_back(dq);
    }

    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(dqCreateInfo.size());
    createInfo.pQueueCreateInfos = dqCreateInfo.data();
    createInfo.queueCreateInfoCount = 1;
    createInfo.pEnabledFeatures = &requestedFeatures;
    createInfo.enabledExtensionCount = 0;

    std::vector<const char*> extPtrs { deviceExtensions.size() };
    for (size_t i = 0; i < deviceExtensions.size(); i++) {
        extPtrs[i] = deviceExtensions[i].c_str();
    }
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extPtrs.size());
    createInfo.ppEnabledExtensionNames = extPtrs.data();

    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else {
        createInfo.enabledLayerCount = 0;
    }
    if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicalDevice) != VK_SUCCESS) {
        throw std::runtime_error("Could not create logical device");
    }
    INFO("Created logical device");
}

VkPhysicalDevice DeviceCandidate::GetPhysicalDevice() const {
    return physicalDevice;
}

VkDevice DeviceCandidate::GetLogicalDevice() const {
    return logicalDevice;
}

VkQueue DeviceCandidate::GetGraphicsQueue() const {
    VkQueue queue;
    vkGetDeviceQueue(logicalDevice, graphicsIndex, 0, &queue);
    return queue;
}

VkQueue DeviceCandidate::GetPresentQueue() const {
    VkQueue queue;
    vkGetDeviceQueue(logicalDevice, presentIndex, 0, &queue);
    return queue;
}

VkSwapchainKHR DeviceCandidate::GetSwapchain() const {
    return swapchain;
}

std::string DeviceCandidate::GetName() const {
    return std::string(properties.deviceName);
}

bool DeviceCandidate::SupportsRequiredExtensions() const {
    uint32_t extCount = 0;
    std::vector<VkExtensionProperties> availableExtensions;

    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extCount, nullptr);
    availableExtensions.resize(extCount);
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extCount, availableExtensions.data());

    unsigned int found = 0;
    for (const auto& e: availableExtensions) {
        if (std::find(deviceExtensions.begin(),
                deviceExtensions.end(),
                e.extensionName) != deviceExtensions.end()) {
            found++;
        }
    }
    return found == deviceExtensions.size();
}


bool DeviceCandidate::QueuesComplete() const {
    return graphicsIndex != -1 && presentIndex != -1;
}

bool DeviceCandidate::SwapchainSuitable() const {
    return SupportsRequiredExtensions() && !surfaceFormats.empty() && !presentModes.empty();
}

bool DeviceCandidate::HasGeometryShader() const {
    return features.geometryShader;
}

bool DeviceCandidate::HasDiscreteGPU() const {
    return properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
}

}
