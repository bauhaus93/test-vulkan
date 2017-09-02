#pragma once

#include <vulkan/vulkan.h>

#include "Shader.h"

namespace engine::vulkan {

class Pipeline {

public:

                            Pipeline(const VkDevice device_, VkExtent2D swapChainExtent);
                            ~Pipeline();

private:
    const VkDevice          device;
    VKRenderPass            renderPass;
    VkPipelineLayout        layout;

    Shader                  vertexShader;
    Shader                  fragmentShader;




    void                    LoadLayout();


};

}
