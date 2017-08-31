#pragma once

#include <vector>
#include <set>
#include <algorithm>
#include <string>
#include <stdexcept>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "ValidationLayer.h"
#include "SwapChain.h"
#include "logging/StdLogger.h"
#include "utility/StringFormat.h"

namespace engine::vulkan {

const std::vector<std::string> deviceExtensions {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

class DeviceCandidate {
private:
    VkPhysicalDevice                        physicalDevice;
    VkDevice                                logicalDevice;
    VkSurfaceKHR                            surface;
    VkSwapchainKHR                          swapchain;
    VkSurfaceCapabilitiesKHR                surfaceCapabilities;
    std::vector<VkSurfaceFormatKHR>         surfaceFormats;
    std::vector<VkPresentModeKHR>           presentModes;
    VkPhysicalDeviceFeatures                features;
    VkPhysicalDeviceProperties              properties;

    int                                     graphicsIndex;
    int                                     presentIndex;

    void                                    LoadQueueFamilies();
    void                                    LoadSwapchainDetails();
public:

                        DeviceCandidate(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface_);

    bool                SwapchainSuitable() const;
    bool                SupportsRequiredExtensions() const;
    bool                QueuesComplete() const;
    bool                HasGeometryShader() const;
    bool                HasDiscreteGPU() const;
    void                CreateLogicalDevice();
    void                CreateSwapchain();

    VkPhysicalDevice    GetPhysicalDevice() const;
    VkDevice            GetLogicalDevice() const;
    VkQueue             GetGraphicsQueue() const;
    VkQueue             GetPresentQueue() const;
    VkSwapchainKHR      GetSwapchain() const;
    std::string         GetName() const;
};

std::vector<DeviceCandidate> GetDeviceCandidates(VkInstance instance, VkSurfaceKHR surface);

}
