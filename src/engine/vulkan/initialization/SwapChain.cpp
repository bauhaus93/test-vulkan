#include "SwapChain.h"

namespace engine::vulkan {

VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFmts) {
    if (availableFmts.size() == 1 && availableFmts[0].format == VK_FORMAT_UNDEFINED) {
        return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
    }

    for (const auto& fmt : availableFmts) {
        if (fmt.format == VK_FORMAT_B8G8R8A8_UNORM &&
            fmt.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return fmt;
        }
    }
    return availableFmts[0];
}

VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availableModes) {
    auto bestMode = VK_PRESENT_MODE_FIFO_KHR;

    for (const auto& mode: availableModes) {
        switch(mode) {
            case VK_PRESENT_MODE_MAILBOX_KHR:   return mode;
            case VK_PRESENT_MODE_IMMEDIATE_KHR: bestMode = mode;    break;
            default:                            break;
        }
    }
    return bestMode;
}

VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, uint32_t width, uint32_t height) {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    }
    else {
        VkExtent2D extent = { width, height };

        extent.width = std::max(
            capabilities.minImageExtent.width,
            std::min(capabilities.maxImageExtent.width, extent.width));

        extent.height = std::max(
            capabilities.minImageExtent.height,
            std::min(capabilities.maxImageExtent.height, extent.height));
        return extent;
    }
}

}
