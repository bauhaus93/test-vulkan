#pragma once



class SwapChain {

public:

                            SwapChain(VkPhysicalDevice physicalDevice,
                                VkDevice logicalDevice,
                                VkSurfaceKHR surface);


private:
    VkSwapchainKHR          swapChain;
    VkSurfaceFormatKHR      imageFormat;
    VkExtent2D              imageExtent;

};
