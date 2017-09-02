#pragma once

#include <string>

#include <vulkan/vulkan.h>

#include "utility/File.h"

namespace engine::vulkan {

class Shader {

public:

    explicit                        Shader(const VkDevice device_, const std::string& filename);
                                    ~Shader();
    VkShaderModule                  GetModule() const { return module; }

private:

    const VkDevice          device;
    VkShaderModule          module;



    void                    LoadModule(const std::string& filename);

};


}
