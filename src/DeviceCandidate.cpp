#include "DeviceCandidate.h"

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
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);
            if (presentSupport) {
                presentIndex = i;
            }
        }
        i++;
    }
}

bool DeviceCandidate::CreateLogicalDevice() {
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

bool DeviceCandidate::QueuesComplete() const {
    return graphicsIndex != -1 && presentIndex != -1;
}

bool DeviceCandidate::HasGeometryShader() const {
    return features.geometryShader;
}

bool DeviceCandidate::HasDiscreteGPU() const {
    return properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
}
