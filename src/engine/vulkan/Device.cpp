#include "Device.h"

namespace engine::vulkan {

std::vector<Device> GetDevices(VkInstance instance, VkSurfaceKHR surface) {
    uint32_t deviceCount = 0;
    std::vector<VkPhysicalDevice> physicalDevices;
    std::vector<Device> devices;

    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (deviceCount > 0) {
      physicalDevices.resize(deviceCount);
      vkEnumeratePhysicalDevices(instance, &deviceCount, physicalDevices.data());
    }
    for (auto device: physicalDevices) {
        devices.emplace_back(device, surface);
    }
    return devices;
}

Device::Device(VkPhysicalDevice physicalDevice_, VkSurfaceKHR surface):
    physicalDevice { physicalDevice_ },
    logicalDevice { VK_NULL_HANDLE },
    graphicsQueue { },
    presentQueue { } {

    LoadQueueFamilyIndices(surface);
}

Device::Device(Device&& other):
    physicalDevice { other.physicalDevice},
    logicalDevice { other.logicalDevice },
    graphicsQueue { other.graphicsQueue },
    presentQueue { other.presentQueue } {

    other.physicalDevice = VK_NULL_HANDLE;
    other.logicalDevice = VK_NULL_HANDLE;
}

Device::~Device() {
    if (logicalDevice != VK_NULL_HANDLE) {
        vkDestroyDevice(logicalDevice, nullptr);
        INFO("Destroyed logical device");
    }
}

void Device::LoadQueueFamilyIndices(VkSurfaceKHR surface) {
    uint32_t queueFamilyCount = 0;
    std::vector<VkQueueFamilyProperties> queueFamilies;

    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
    queueFamilies.resize(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto& qf: queueFamilies) {
        if (qf.queueCount > 0) {
            if (qf.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                graphicsQueue.SetIndex(i);
            }
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);
            if (presentSupport) {
                presentQueue.SetIndex(i);
            }
        }
        i++;
    }
}

void Device::LoadQueueFamilyQueues() {
    VkQueue grQueue;
    vkGetDeviceQueue(logicalDevice, graphicsQueue.GetIndex(), 0, &grQueue);
    graphicsQueue.SetQueue(grQueue);

    VkQueue prQueue;
    vkGetDeviceQueue(logicalDevice, presentQueue.GetIndex(), 0, &prQueue);
    presentQueue.SetQueue(prQueue);
}

void Device::LoadDeviceQueueInfo(VkDeviceCreateInfo& createInfo, std::vector<VkDeviceQueueCreateInfo>& dqCreateInfo) {
    std::set<int> uniqueQueueFamilies = { graphicsQueue.GetIndex(), presentQueue.GetIndex() };
    static float queuePriority = 1.0f;

    for (int qf: uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo dq {};
        dq.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        dq.queueFamilyIndex = qf;
        dq.queueCount = 1;
        dq.pQueuePriorities = &queuePriority;
        dqCreateInfo.push_back(dq);
    }
    createInfo.pQueueCreateInfos = dqCreateInfo.data();
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(dqCreateInfo.size());
}

void Device::LoadDeviceExtensions(VkDeviceCreateInfo& createInfo) {
    createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredDeviceExtensions.size());
    createInfo.ppEnabledExtensionNames = requiredDeviceExtensions.data();
}

void Device::LoadValidationLayers(VkDeviceCreateInfo& createInfo) {
    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else {
        createInfo.enabledLayerCount = 0;
    }
}

void Device::LoadLogicalDevice() {
    VkDeviceCreateInfo createInfo {};
    std::vector<VkDeviceQueueCreateInfo> dqCreateInfo;
    VkPhysicalDeviceFeatures requestedFeatures { VK_FALSE };

    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pEnabledFeatures = &requestedFeatures;

    LoadDeviceQueueInfo(createInfo, dqCreateInfo);
    LoadDeviceExtensions(createInfo);
    LoadValidationLayers(createInfo);

    if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicalDevice) != VK_SUCCESS) {
        throw std::runtime_error("Could not create logical device");
    }
    INFO("Created logical device");
    LoadQueueFamilyQueues();

}

std::unique_ptr<SwapChain> Device::CreateSwapChain(VkSurfaceKHR surface) {
    return std::make_unique<SwapChain>(physicalDevice,
        logicalDevice,
        surface,
        graphicsQueue.GetIndex(),
        presentQueue.GetIndex());
}

bool Device::SupportsRequiredExtensions() const {
    uint32_t extCount = 0;
    std::vector<VkExtensionProperties> availableExtensions;
    std::vector<std::string> requiredExtensions;

    for(const auto& ext: requiredDeviceExtensions) {
        requiredExtensions.emplace_back(ext);
    }

    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extCount, nullptr);
    availableExtensions.resize(extCount);
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extCount, availableExtensions.data());

    unsigned int found = 0;
    for (const auto& e: availableExtensions) {
        if (std::find(requiredExtensions.begin(),
                requiredExtensions.end(),
                e.extensionName) != requiredExtensions.end()) {
            found++;
        }
    }
    return found == requiredExtensions.size();
}

bool Device::QueuesComplete() const {
    return graphicsQueue.GetIndex() != -1 && presentQueue.GetIndex() != -1;
}

std::string Device::GetName() const {
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(physicalDevice, &properties);
    return std::string(properties.deviceName);
}

}
