#pragma once

#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "utility/ValidationLayer.h"

class DeviceCandidate {
private:
    VkPhysicalDevice                        physicalDevice;
    VkDevice                                logicalDevice;
    VkPhysicalDeviceFeatures                features;
    VkPhysicalDeviceProperties              properties;
    int                                     graphicsIndex;

    void                                    LoadQueueFamilies();
public:

    explicit            DeviceCandidate(VkPhysicalDevice device_);

    bool                QueuesComplete() const;
    bool                HasGeometryShader() const;
    bool                HasDiscreteGPU() const;
    bool                CreateLogicalDevice();

    VkPhysicalDevice    GetPhysicalDevice() const;
    VkDevice            GetLogicalDevice() const;
};

std::vector<DeviceCandidate> GetDeviceCandidates(VkInstance instance);
