#include "SwapChain.h"

namespace engine::vulkan {

SwapChain::SwapChain(VkPhysicalDevice physicalDevice, const VkDevice logicalDevice_, VkSurfaceKHR surface, int graphicsIndex, int presentIndex):
    swapChain { VK_NULL_HANDLE },
    logicalDevice { logicalDevice_ } {

    LoadSurfaceFormat(physicalDevice, surface);
    LoadPresentMode(physicalDevice, surface);
    LoadSwapExent(physicalDevice, surface, 800, 600);
    LoadSwapChain(physicalDevice, surface, graphicsIndex, presentIndex);
    LoadImages();
    INFO("Created swapchain");
}

SwapChain::SwapChain(SwapChain&& other):
    swapChain { other.swapChain },
    logicalDevice { other.logicalDevice },
    imageFormat { other.imageFormat },
    presentMode { other.presentMode },
    imageExtent { other.imageExtent },
    images { std::move(images) } {

    other.swapChain = VK_NULL_HANDLE;
    INFO("Moved swapchain");
}

SwapChain::~SwapChain() {
    if (swapChain != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(logicalDevice, swapChain, nullptr);
        INFO("Destroyed swapchain");
    }
}

void SwapChain::LoadSurfaceFormat(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
    uint32_t formatCount;
    std::vector<VkSurfaceFormatKHR> availableFmts;

    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
    if (formatCount != 0) {
        availableFmts.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, availableFmts.data());
    }

    if (availableFmts.size() == 1 && availableFmts[0].format == VK_FORMAT_UNDEFINED) {
        imageFormat = { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
    }
    else {
        for (const auto& fmt : availableFmts) {
            if (fmt.format == VK_FORMAT_B8G8R8A8_UNORM &&
                fmt.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                imageFormat = fmt;
            }
        }
    }
}

void SwapChain::LoadPresentMode(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
    uint32_t presentModeCount;
    std::vector<VkPresentModeKHR> availablePresentModes;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);
    if (presentModeCount != 0) {
        availablePresentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, availablePresentModes.data());
    }

    auto bestMode = VK_PRESENT_MODE_FIFO_KHR;
    for (const auto& mode: availablePresentModes) {
        switch(mode) {
        case VK_PRESENT_MODE_MAILBOX_KHR:   presentMode = mode; return;
        case VK_PRESENT_MODE_IMMEDIATE_KHR: bestMode = mode;    break;
        default:                            break;
        }
    }
    presentMode = bestMode;
}

void SwapChain::LoadSwapExent(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, uint32_t width, uint32_t height) {
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities);

    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        imageExtent = capabilities.currentExtent;
    }
    else {
        VkExtent2D extent = { width, height };

        extent.width = std::max(
            capabilities.minImageExtent.width,
            std::min(capabilities.maxImageExtent.width, extent.width));

        extent.height = std::max(
            capabilities.minImageExtent.height,
            std::min(capabilities.maxImageExtent.height, extent.height));
        imageExtent = extent;
    }
}

void SwapChain::LoadSwapChain(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, int graphicsIndex, int presentIndex) {
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities);

    uint32_t imageCount = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount) {
        imageCount = capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = imageFormat.format;
    createInfo.imageColorSpace = imageFormat.colorSpace;
    createInfo.imageExtent = imageExtent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    uint32_t familyIndices[] = {
        static_cast<uint32_t>(graphicsIndex),
        static_cast<uint32_t>(presentIndex) };

    if (graphicsIndex != presentIndex) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = familyIndices;
    }
    else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
    }

    createInfo.preTransform = capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(logicalDevice, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
        throw std::runtime_error("Could not create swapchain");
    }
}

void SwapChain::LoadImages() {
    uint32_t imageCount;
    vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, nullptr);
    images.resize(imageCount);
    vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, images.data());
}

}
