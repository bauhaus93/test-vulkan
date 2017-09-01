#pragma once

#include <vector>
#include <algorithm>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "logging/StdLogger.h"

namespace engine::vulkan {

class SwapChain {

public:

                        SwapChain(VkPhysicalDevice physicalDevice,
                            const VkDevice logicalDevice_,
                            VkSurfaceKHR surface,
                            int graphicsIndex,
                            int presentIndex);
                        SwapChain(SwapChain&& other);
                        ~SwapChain();


private:
    VkSwapchainKHR          swapChain;
    const VkDevice          logicalDevice;
    VkSurfaceFormatKHR      imageFormat;
    VkPresentModeKHR        presentMode;
    VkExtent2D              imageExtent;

    std::vector<VkImage>    images;


    void                LoadSurfaceFormat(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
    void                LoadPresentMode(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
    void                LoadSwapExent(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, uint32_t width, uint32_t height);
    void                LoadSwapChain(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, int graphicsIndex, int presentIndex);
    void                LoadImages();

};

}
