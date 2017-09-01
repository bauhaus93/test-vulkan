#pragma once

#include <memory>
#include <set>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "utility/StringFormat.h"
#include "initialization/ValidationLayer.h"

#include "SwapChain.h"
#include "Queue.h"

namespace engine::vulkan {

const std::vector<const char*> requiredDeviceExtensions {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

class Device {

public:

    explicit                        Device(VkPhysicalDevice physicalDevice_, VkSurfaceKHR surface);
                                    Device(Device&& other);
                                    ~Device();
    void                            LoadLogicalDevice();
    void                            LoadSwapChain(VkSurfaceKHR surface);

    bool                            SupportsRequiredExtensions() const;
    bool                            QueuesComplete() const;
    std::string                     GetName() const;

private:
    VkPhysicalDevice                physicalDevice;
    VkDevice                        logicalDevice;
    std::unique_ptr<SwapChain>      swapchain;

    Queue                           graphicsQueue;
    Queue                           presentQueue;

    void                            LoadQueueFamilyIndices(VkSurfaceKHR surface);
    void                            LoadQueueFamilyQueues();
    void                            LoadDeviceExtensions(VkDeviceCreateInfo& createInfo);
    void                            LoadValidationLayers(VkDeviceCreateInfo& createInfo);
    void                            LoadDeviceQueueInfo(VkDeviceCreateInfo& createInfo, std::vector<VkDeviceQueueCreateInfo>& dqCreateInfo);

};

std::vector<Device> GetDevices(VkInstance instance, VkSurfaceKHR surface);

}
