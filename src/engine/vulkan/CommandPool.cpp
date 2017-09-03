#include "CommandPool.h"

namespace engine::vulkan {

CommandPool::CommandPool(VkDevice device_, const Queue& queue):
    device { device_ },
    pool { VK_NULL_HANDLE } {

    VkCommandPoolCreateInfo createInfo {};
    createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    createInfo.queueFamilyIndex = queue.GetIndex();
    createInfo.flags = 0;

    if (vkCreateCommandPool(device, &createInfo, nullptr, &pool) != VK_SUCCESS) {
        throw std::runtime_error("Could not create command pool");
    }
    DEBUG("Created command pool");
}

CommandPool::~CommandPool() {

    if (buffers.size() > 0) {
        vkFreeCommandBuffers(device, pool, buffers.size(), buffers.data());
        DEBUG("Destroyed command buffers");
    }

    if (pool != VK_NULL_HANDLE) {
        vkDestroyCommandPool(device, pool, nullptr);
        DEBUG("Destroyed command pool");
    }
}

void CommandPool::LoadCommandBuffers(const SwapChain& swapchain) {
    buffers.resize(swapchain.GetFramebufferCount());
    VkCommandBufferAllocateInfo allocInfo {};

    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = pool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = static_cast<uint32_t>(buffers.size());

    if (vkAllocateCommandBuffers(device, &allocInfo, buffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("Could not create command buffers");
    }
    DEBUG("Created command buffers");
}

void CommandPool::RecordCommand(const SwapChain& swapchain, const Pipeline& pipeline) {

    for(size_t i = 0; i < buffers.size(); i++) {
        VkCommandBufferBeginInfo beginInfo {};

        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
        beginInfo.pInheritanceInfo = nullptr;

        vkBeginCommandBuffer(buffers[i], &beginInfo);

        VkRenderPassBeginInfo rpBeginInfo {};
        rpBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        rpBeginInfo.renderPass = pipeline.GetRenderPass();
        rpBeginInfo.framebuffer = swapchain.GetFramebuffer(i);
        rpBeginInfo.renderArea.offset = { 0, 0 };
        rpBeginInfo.renderArea.extent = swapchain.GetImageExtent();

        VkClearValue clearColor { 0.0f, 0.0f, 0.0f, 1.0f };
        rpBeginInfo.clearValueCount = 1;
        rpBeginInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(buffers[i], &rpBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.GetPipeline());
        vkCmdDraw(buffers[i], 3, 1, 0, 0);

        vkCmdEndRenderPass(buffers[i]);

        if (vkEndCommandBuffer(buffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("Could not record command buffer");
        }
    }
    DEBUG("Recorded command");
}

}
