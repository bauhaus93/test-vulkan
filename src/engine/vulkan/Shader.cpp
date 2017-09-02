#include "Shader.h"

namespace engine::vulkan {

Shader::Shader(const VkDevice device_, const std::string& filename):
    device { device_ },
    module { VK_NULL_HANDLE } {

    LoadModule(filename);
}

Shader::~Shader() {
    if (module != VK_NULL_HANDLE) {
        vkDestroyShaderModule(device, module, nullptr);
    }
}

void Shader::LoadModule(const std::string& filename) {
    std::vector<char> code { ReadFile(filename) };
    VkShaderModuleCreateInfo createInfo {};

    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    if (vkCreateShaderModule(device, &createInfo, nullptr, &module) != VK_SUCCESS) {
        throw std::runtime_error("Could not create shader module");
    }
}

}
