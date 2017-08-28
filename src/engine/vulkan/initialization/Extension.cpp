#include "Extension.h"

namespace engine::vulkan {

void ListAvailableVulkanExtensions() {
  uint32_t extCount = 0;

  vkEnumerateInstanceExtensionProperties(nullptr, &extCount, nullptr);
  std::vector<VkExtensionProperties> extensions(extCount);

  vkEnumerateInstanceExtensionProperties(nullptr, &extCount, extensions.data());
  std::cout << "Available extensions:" << std::endl;

  for (const auto& e: extensions) {
    std::cout << "  " << e.extensionName << std::endl;
  }
}

std::vector<const char*> GetRequiredExtensions() {
  std::vector<const char*> extensions;

  unsigned int glfwExtCount = 0;
  const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtCount);

  for (unsigned int i = 0; i < glfwExtCount; i++) {
    extensions.push_back(glfwExtensions[i]);
  }

  if (enableValidationLayers) {
    extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
  }

  return extensions;
}

}
