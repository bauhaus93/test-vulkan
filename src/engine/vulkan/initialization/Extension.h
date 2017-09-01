#pragma once

#include <stdexcept>
#include <vector>
#include <iostream>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "ValidationLayer.h"
#include "logging/StdLogger.h"

namespace engine::vulkan {

void ListAvailableVulkanExtensions();
std::vector<const char*> GetRequiredExtensions();

}
