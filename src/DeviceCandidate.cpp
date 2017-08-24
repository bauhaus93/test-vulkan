#include "DeviceCandidate.h"

std::vector<DeviceCandidate> GetDeviceCandidates(VkInstance instance) {
    uint32_t deviceCount = 0;
    std::vector<VkPhysicalDevice> devices;
    std::vector<DeviceCandidate> wrappedDevices;

    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (deviceCount > 0) {
      devices.resize(deviceCount);
      vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
    }
    for (auto device: devices) {
        wrappedDevices.emplace_back(device);
    }
    return wrappedDevices;
}

DeviceCandidate::DeviceCandidate(VkPhysicalDevice physicalDevice_):
    physicalDevice { physicalDevice_ },
    logicalDevice { VK_NULL_HANDLE },
    graphicsIndex { -1 } {
    vkGetPhysicalDeviceFeatures(physicalDevice, &features);
    vkGetPhysicalDeviceProperties(physicalDevice, &properties);
    LoadQueueFamilies();
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
        }
        i++;
    }
}

bool DeviceCandidate::CreateLogicalDevice() {
    VkDeviceQueueCreateInfo dqCreateInfo {};
    VkDeviceCreateInfo createInfo {};
    VkPhysicalDeviceFeatures requestedFeatures { VK_FALSE };
    float queuePriority = 1.0f;

    dqCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    dqCreateInfo.queueFamilyIndex = graphicsIndex;
    dqCreateInfo.queueCount = 1;
    dqCreateInfo.pQueuePriorities = &queuePriority;

    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = &dqCreateInfo;
    createInfo.queueCreateInfoCount = 1;
    createInfo.pEnabledFeatures = &requestedFeatures;
    createInfo.enabledExtensionCount = 0;

    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else {
        createInfo.enabledLayerCount = 0;
    }

    return vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicalDevice) == VK_SUCCESS;
}

VkPhysicalDevice DeviceCandidate::GetPhysicalDevice() const {
    return physicalDevice;
}

VkDevice DeviceCandidate::GetLogicalDevice() const {
    return logicalDevice;
}

bool DeviceCandidate::QueuesComplete() const {
    return graphicsIndex != -1;
}

bool DeviceCandidate::HasGeometryShader() const {
    return features.geometryShader;
}

bool DeviceCandidate::HasDiscreteGPU() const {
    return properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
}
