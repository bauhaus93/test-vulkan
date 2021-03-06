#pragma once

#include <vector>
#include <algorithm>

#include <vulkan/vulkan.h>

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

    void                        LoadFramebuffers(VkRenderPass renderPass);

    VkSurfaceFormatKHR          GetImageFormat() const { return imageFormat; }
    VkExtent2D                  GetImageExtent() const { return imageExtent; }
    size_t                      GetFramebufferCount() const { return framebuffers.size(); }
    VkFramebuffer               GetFramebuffer(size_t index) const { return framebuffers[index]; }
    VkSwapchainKHR              GetSwapChain() const { return swapChain; };


private:
    VkSwapchainKHR              swapChain;
    const VkDevice              logicalDevice;
    VkSurfaceFormatKHR          imageFormat;
    VkPresentModeKHR            presentMode;
    VkExtent2D                  imageExtent;

    std::vector<VkImage>        images;
    std::vector<VkImageView>    imageViews;
    std::vector<VkFramebuffer>  framebuffers;


    void                LoadSurfaceFormat(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
    void                LoadPresentMode(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
    void                LoadSwapExent(VkPhysicalDevice physicalDevice,
                                        VkSurfaceKHR surface,
                                        uint32_t width,
                                        uint32_t height);
    void                LoadSwapChain(VkPhysicalDevice physicalDevice,
                                        VkSurfaceKHR surface,
                                        int graphicsIndex,
                                        int presentIndex);
    void                LoadImages();
    void                LoadImageViews();

};

}
