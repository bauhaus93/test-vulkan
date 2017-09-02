#include "Pipeline.h"

namespace engine::vulkan {

static VkAttachmentDescription CreateAttachmentDescription(VkSurfaceFormatKHR imageFormat);
static VkAttachmentReference CreateAttachmentReference();
static VkSubpassDescription CreateSubpassDescription(VkAttachmentReference* attachRef);
static VkRenderPassCreateInfo CreateRenderPassInfo(VkAttachmentDescription* attachDescr, VkSubpassDescription* subpassDescr);
static VkPipelineShaderStageCreateInfo CreateVertexShaderStageInfo(VkShaderModule module);
static VkPipelineShaderStageCreateInfo CreateFragmentShaderStageInfo(VkShaderModule module);
static VkPipelineVertexInputStateCreateInfo CreateVertexInputStateInfo();
static VkPipelineInputAssemblyStateCreateInfo CreateInputAssemblyStateInfo();
static VkViewport CreateViewport(VkExtent2D swapChainExtent);
static VkRect2D CreaterScissorRect(VkExtent2D swapChainExtent);
static VkPipelineViewportStateCreateInfo CreateViewportStateInfo(VkViewport* viewport, VkRect2D* scissor);
static VkPipelineRasterizationStateCreateInfo CreateRasterizerStateInfo();
static VkPipelineMultisampleStateCreateInfo CreateMultisampleStateInfo();
static VkPipelineColorBlendAttachmentState CreateColorBlendAttachInfo();
static VkPipelineColorBlendStateCreateInfo CreateColorBlendStateInfo(VkPipelineColorBlendAttachmentState* attachInfo);
//static VkPipelineDynamicStateCreateInfo CreateDynamicStateInfo(VkDynamicState* dynamicStates, size_t dsCount);
static VkPipelineLayoutCreateInfo CreatePipelineLayoutInfo();
static VkGraphicsPipelineCreateInfo CreatePipelineInfo(VkPipelineShaderStageCreateInfo* shaderStageInfos,
    VkPipelineVertexInputStateCreateInfo* vertexInputInfo,
    VkPipelineInputAssemblyStateCreateInfo* inputAssemblyInfo,
    VkPipelineViewportStateCreateInfo* viewportStateInfo,
    VkPipelineRasterizationStateCreateInfo* rasterStateInfo,
    VkPipelineMultisampleStateCreateInfo* multisampleStateInfo,
    VkPipelineColorBlendStateCreateInfo* blendStateInfo,
    VkPipelineLayout layout,
    VkRenderPass renderPass);

Pipeline::Pipeline(const VkDevice device_, VkExtent2D swapChainExtent, VkSurfaceFormatKHR swapChainFormat):
    device { device_ },
    renderPass { VK_NULL_HANDLE },
    layout { VK_NULL_HANDLE },
    pipeline { VK_NULL_HANDLE },
    vertexShader { device, "vert.spv" },
    fragmentShader { device, "frag.spv" } {

    VkAttachmentDescription attachDescr { CreateAttachmentDescription(swapChainFormat) };
    VkAttachmentReference attachRef { CreateAttachmentReference() };
    VkSubpassDescription subpassDescr { CreateSubpassDescription(&attachRef) };

    LoadRenderPass(&attachDescr, &subpassDescr);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo { CreateVertexShaderStageInfo(vertexShader.GetModule()) };
    VkPipelineShaderStageCreateInfo fragShaderStageInfo { CreateFragmentShaderStageInfo(fragmentShader.GetModule()) };
    VkPipelineVertexInputStateCreateInfo vertInputInfo { CreateVertexInputStateInfo() };
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo { CreateInputAssemblyStateInfo() };
    VkViewport viewport { CreateViewport(swapChainExtent) };
    VkRect2D scissorRect { CreaterScissorRect(swapChainExtent) };
    VkPipelineViewportStateCreateInfo viewportInfo { CreateViewportStateInfo(&viewport, &scissorRect) };
    VkPipelineRasterizationStateCreateInfo rasterInfo { CreateRasterizerStateInfo() };
    VkPipelineMultisampleStateCreateInfo multisampleInfo { CreateMultisampleStateInfo() };
    VkPipelineColorBlendAttachmentState blendAttachInfo { CreateColorBlendAttachInfo() };
    VkPipelineColorBlendStateCreateInfo blendInfo { CreateColorBlendStateInfo(&blendAttachInfo) };
    //VkPipelineDynamicStateCreateInfo dynamicStateInfo { CreateDynamicStateInfo(nullptr, 0) };
    VkPipelineLayoutCreateInfo pipelineLayoutInfo { CreatePipelineLayoutInfo() };

    if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &layout) != VK_SUCCESS) {
        throw std::runtime_error("Could not create pipeline layout");
    }
    VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };
    VkGraphicsPipelineCreateInfo pipelineInfo { CreatePipelineInfo(
        shaderStages,
        &vertInputInfo,
        &inputAssemblyInfo,
        &viewportInfo,
        &rasterInfo,
        &multisampleInfo,
        &blendInfo,
        layout,
        renderPass) };

    if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS) {
        throw std::runtime_error("Could not create graphics pipeline");
    }
    INFO("Created graphics pipeline");

}

Pipeline::~Pipeline() {

    if (pipeline != VK_NULL_HANDLE) {
        vkDestroyPipeline(device, pipeline, nullptr);
        INFO("Destroyed graphics pipeline");
    }

    if (layout != VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(device, layout, nullptr);
    }

    if (renderPass != VK_NULL_HANDLE) {
        vkDestroyRenderPass(device, renderPass, nullptr);
    }

}

void Pipeline::LoadRenderPass(VkAttachmentDescription* attachDescr, VkSubpassDescription* subpassDescr) {
    VkRenderPassCreateInfo createInfo { CreateRenderPassInfo(attachDescr, subpassDescr) };

    if (vkCreateRenderPass(device, &createInfo, nullptr, &renderPass) != VK_SUCCESS) {
        throw std::runtime_error("Could not create render pass");
    }
}

static VkAttachmentDescription CreateAttachmentDescription(VkSurfaceFormatKHR imageFormat) {
    VkAttachmentDescription attachDescr {};

    attachDescr.format = imageFormat.format;
    attachDescr.samples = VK_SAMPLE_COUNT_1_BIT;

    attachDescr.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachDescr.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    attachDescr.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachDescr.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    attachDescr.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachDescr.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    return attachDescr;
}

static VkAttachmentReference CreateAttachmentReference() {
    VkAttachmentReference attachRef {};

    attachRef.attachment = 0;
    attachRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    return attachRef;
}

static VkSubpassDescription CreateSubpassDescription(VkAttachmentReference* attachRef) {
    VkSubpassDescription subpassDescr {};

    subpassDescr.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescr.colorAttachmentCount = 1;
    subpassDescr.pColorAttachments = attachRef;

    return subpassDescr;
}

static VkRenderPassCreateInfo CreateRenderPassInfo(VkAttachmentDescription* attachDescr, VkSubpassDescription* subpassDescr) {
    VkRenderPassCreateInfo createInfo {};

    createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    createInfo.attachmentCount = 1;
    createInfo.pAttachments = attachDescr;
    createInfo.subpassCount = 1;
    createInfo.pSubpasses = subpassDescr;

    return createInfo;
}


static VkPipelineShaderStageCreateInfo CreateVertexShaderStageInfo(VkShaderModule module) {
    VkPipelineShaderStageCreateInfo createInfo {};

    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    createInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    createInfo.module = module;
    createInfo.pName = "main";

    return createInfo;
}

static VkPipelineShaderStageCreateInfo CreateFragmentShaderStageInfo(VkShaderModule module) {
    VkPipelineShaderStageCreateInfo createInfo {};

    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    createInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    createInfo.module = module;
    createInfo.pName = "main";

    return createInfo;
}

static VkPipelineVertexInputStateCreateInfo CreateVertexInputStateInfo() {
    VkPipelineVertexInputStateCreateInfo createInfo {};

    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    createInfo.vertexBindingDescriptionCount = 0;
    createInfo.pVertexBindingDescriptions = nullptr;
    createInfo.vertexAttributeDescriptionCount = 0;
    createInfo.pVertexAttributeDescriptions = nullptr;

    return createInfo;
}

static VkPipelineInputAssemblyStateCreateInfo CreateInputAssemblyStateInfo() {
    VkPipelineInputAssemblyStateCreateInfo createInfo {};

    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    createInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    createInfo.primitiveRestartEnable = VK_FALSE;

    return createInfo;
}

static VkViewport CreateViewport(VkExtent2D swapChainExtent) {
    VkViewport viewport {};

    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(swapChainExtent.width);
    viewport.height = static_cast<float>(swapChainExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    return viewport;
}

static VkRect2D CreaterScissorRect(VkExtent2D swapChainExtent) {
    VkRect2D scissor {};

    scissor.offset = { 0, 0 };
    scissor.extent = swapChainExtent;

    return scissor;
}

static VkPipelineViewportStateCreateInfo CreateViewportStateInfo(VkViewport* viewport, VkRect2D* scissor) {
    VkPipelineViewportStateCreateInfo createInfo {};

    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    createInfo.viewportCount = 1;
    createInfo.pViewports = viewport;
    createInfo.scissorCount = 1;
    createInfo.pScissors = scissor;

    return createInfo;
}

static VkPipelineRasterizationStateCreateInfo CreateRasterizerStateInfo() {
    VkPipelineRasterizationStateCreateInfo createInfo {};

    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    createInfo.depthClampEnable = VK_FALSE;           //other needs gpu feature enabled
    createInfo.rasterizerDiscardEnable = VK_FALSE;
    createInfo.polygonMode = VK_POLYGON_MODE_FILL;  //other needs gpu feature enabled
    createInfo.lineWidth = 1.0f;                    // > 1.0 needs gpu feature enabled (wideLines)
    createInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    createInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;

    createInfo.depthBiasEnable = VK_FALSE;
    createInfo.depthBiasConstantFactor = 0.0f;
    createInfo.depthBiasClamp = 0.0f;
    createInfo.depthBiasSlopeFactor = 0.0f;

    return createInfo;
}

static VkPipelineMultisampleStateCreateInfo CreateMultisampleStateInfo() {
    VkPipelineMultisampleStateCreateInfo createInfo {};

    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    createInfo.sampleShadingEnable = VK_FALSE;
    createInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    createInfo.minSampleShading = 1.0f;
    createInfo.pSampleMask = nullptr;
    createInfo.alphaToCoverageEnable = VK_FALSE;
    createInfo.alphaToOneEnable = VK_FALSE;

    return createInfo;
}

static VkPipelineColorBlendAttachmentState CreateColorBlendAttachInfo() {
    VkPipelineColorBlendAttachmentState colorBlend {};

    colorBlend.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
        VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT |
        VK_COLOR_COMPONENT_A_BIT;

    colorBlend.blendEnable = VK_FALSE;
    colorBlend.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlend.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlend.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlend.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlend.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlend.alphaBlendOp = VK_BLEND_OP_ADD;

    return colorBlend;
}

static VkPipelineColorBlendStateCreateInfo CreateColorBlendStateInfo(VkPipelineColorBlendAttachmentState* attachInfo) {
    VkPipelineColorBlendStateCreateInfo createInfo {};

    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    createInfo.logicOpEnable = VK_FALSE;
    createInfo.logicOp = VK_LOGIC_OP_COPY;
    createInfo.attachmentCount = 1;
    createInfo.pAttachments = attachInfo;
    createInfo.blendConstants[0] = 0.0f;
    createInfo.blendConstants[1] = 0.0f;
    createInfo.blendConstants[2] = 0.0f;
    createInfo.blendConstants[3] = 0.0f;

    return createInfo;
}

/*static VkPipelineDynamicStateCreateInfo CreateDynamicStateInfo(VkDynamicState* dynamicStates, size_t dsCount) {
    VkPipelineDynamicStateCreateInfo createInfo {};

    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    createInfo.dynamicStateCount = dsCount;
    createInfo.pDynamicStates = dynamicStates;

    return createInfo;
}*/

static VkPipelineLayoutCreateInfo CreatePipelineLayoutInfo() {
    VkPipelineLayoutCreateInfo createInfo {};

    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    createInfo.setLayoutCount = 0;
    createInfo.pSetLayouts = nullptr;
    createInfo.pushConstantRangeCount = 0;
    createInfo.pPushConstantRanges = nullptr;

    return createInfo;
}

static VkGraphicsPipelineCreateInfo CreatePipelineInfo(VkPipelineShaderStageCreateInfo* shaderStageInfos,
    VkPipelineVertexInputStateCreateInfo* vertexInputInfo,
    VkPipelineInputAssemblyStateCreateInfo* inputAssemblyInfo,
    VkPipelineViewportStateCreateInfo* viewportStateInfo,
    VkPipelineRasterizationStateCreateInfo* rasterStateInfo,
    VkPipelineMultisampleStateCreateInfo* multisampleStateInfo,
    VkPipelineColorBlendStateCreateInfo* blendStateInfo,
    VkPipelineLayout layout,
    VkRenderPass renderPass) {

    VkGraphicsPipelineCreateInfo createInfo {};

    createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    createInfo.stageCount = 2;
    createInfo.pStages = shaderStageInfos;
    createInfo.pVertexInputState = vertexInputInfo;
    createInfo.pInputAssemblyState = inputAssemblyInfo;
    createInfo.pViewportState = viewportStateInfo;
    createInfo.pRasterizationState = rasterStateInfo;
    createInfo.pMultisampleState = multisampleStateInfo;
    createInfo.pDepthStencilState = nullptr;
    createInfo.pColorBlendState = blendStateInfo;
    createInfo.pDynamicState = nullptr;

    createInfo.layout = layout;
    createInfo.renderPass = renderPass;
    createInfo.subpass = 0;

    createInfo.basePipelineHandle = VK_NULL_HANDLE;
    createInfo.basePipelineIndex = -1;

    return createInfo;
}


}
