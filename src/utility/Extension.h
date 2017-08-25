#pragma once

#include <stdexcept>
#include <vector>
#include <iostream>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "ValidationLayer.h"

const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

void ListAvailableVulkanExtensions();
std::vector<const char*> GetRequiredExtensions();
