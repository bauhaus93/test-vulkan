#pragma once

#include <vulkan/vulkan.h>

#include "logging/StdLogger.h"
#include "Shader.h"

namespace engine::vulkan {

class Pipeline {

public:

                            Pipeline(const VkDevice device_,
                                VkExtent2D swapChainExtent,
                                VkSurfaceFormatKHR swapChainFormat);
                            ~Pipeline();
    VkRenderPass            GetRenderPass() const { return renderPass; }
    VkPipeline              GetPipeline() const { return pipeline; }

private:
    const VkDevice          device;
    VkRenderPass            renderPass;
    VkPipelineLayout        layout;
    VkPipeline              pipeline;

    Shader                  vertexShader;
    Shader                  fragmentShader;



    void                    LoadRenderPass(VkAttachmentDescription* attachDescr, VkSubpassDescription* subpassDescr);
    void                    LoadLayout();


};

}
