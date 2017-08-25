#pragma once

#include <vector>
#include <set>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "utility/ValidationLayer.h"

class DeviceCandidate {
private:
    VkPhysicalDevice                        physicalDevice;
    VkDevice                                logicalDevice;
    VkSurfaceKHR                            surface;
    VkPhysicalDeviceFeatures                features;
    VkPhysicalDeviceProperties              properties;
    int                                     graphicsIndex;
    int                                     presentIndex;

    void                                    LoadQueueFamilies();
public:

                        DeviceCandidate(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface_);

    bool                QueuesComplete() const;
    bool                HasGeometryShader() const;
    bool                HasDiscreteGPU() const;
    bool                CreateLogicalDevice();

    VkPhysicalDevice    GetPhysicalDevice() const;
    VkDevice            GetLogicalDevice() const;
    VkQueue             GetGraphicsQueue() const;
    VkQueue             GetPresentQueue() const;
};

std::vector<DeviceCandidate> GetDeviceCandidates(VkInstance instance, VkSurfaceKHR surface);
