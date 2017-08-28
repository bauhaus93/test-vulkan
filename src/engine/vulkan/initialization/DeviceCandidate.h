#pragma once

#include <vector>
#include <set>
#include <algorithm>
#include <string>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "ValidationLayer.h"
#include "logging/StdLogger.h"

namespace engine::vulkan {

const std::vector<std::string> deviceExtensions {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

class DeviceCandidate {
private:
    VkPhysicalDevice                        physicalDevice;
    VkDevice                                logicalDevice;
    VkSurfaceKHR                            surface;
    VkSurfaceCapabilitiesKHR                surfaceCapabilities;
    std::vector<VkSurfaceFormatKHR>         surfaceFormats;
    std::vector<VkPresentModeKHR>           presentModes;
    VkPhysicalDeviceFeatures                features;
    VkPhysicalDeviceProperties              properties;

    int                                     graphicsIndex;
    int                                     presentIndex;

    void                                    LoadQueueFamilies();
    void                                    LoadSwapChainDetails();
public:

                        DeviceCandidate(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface_);

    bool                SwapChainSuitable() const;
    bool                SupportsRequiredExtensions() const;
    bool                QueuesComplete() const;
    bool                HasGeometryShader() const;
    bool                HasDiscreteGPU() const;
    bool                CreateLogicalDevice();

    VkPhysicalDevice    GetPhysicalDevice() const;
    VkDevice            GetLogicalDevice() const;
    VkQueue             GetGraphicsQueue() const;
    VkQueue             GetPresentQueue() const;
    std::string         GetName() const;
};

std::vector<DeviceCandidate> GetDeviceCandidates(VkInstance instance, VkSurfaceKHR surface);

}
