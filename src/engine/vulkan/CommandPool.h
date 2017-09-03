#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include "SwapChain.h"
#include "Pipeline.h"
#include "Queue.h"

namespace engine::vulkan {

class CommandPool {

public:

                            CommandPool(VkDevice device_, const Queue& queue);
                            ~CommandPool();
    void                    LoadCommandBuffers(const SwapChain& swapchain);
    void                    RecordCommand(const SwapChain& swapchain, const Pipeline& pipeline);

private:

    VkDevice                        device;
    VkCommandPool                   pool;
    std::vector<VkCommandBuffer>    buffers;


};

}
