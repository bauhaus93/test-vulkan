#pragma once

#include <stdexcept>
#include <vector>
#include <iostream>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "ValidationLayer.h"

void ListAvailableVulkanExtensions();
std::vector<const char*> GetRequiredExtensions();
