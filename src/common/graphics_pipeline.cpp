#include "graphics_pipeline.h"

#include <fstream>

// Loading a shader.
static std::vector<char> readFile(const std::string &filename) {
  std::ifstream file(filename, std::ios::ate | std::ios::binary);

  if (!file.is_open()) {
    throw std::runtime_error("Error: failed to open file!");
  }

  size_t fileSize = (size_t)file.tellg();
  std::vector<char> buffer(fileSize);

  file.seekg(0);
  file.read(buffer.data(), fileSize);

  file.close();

  return buffer;
}

// Before we can pass the codeBuffer to the pipeline, we have to wrap it in a
// VkShaderModule object.
ShaderStageInfo::ShaderStageInfo(const Device::Ptr &device,
                                 const std::string &fileName,
                                 VkShaderStageFlagBits shaderStage,
                                 const std::string &entryPoint) {
  mDevice = device;
  mShaderStage = shaderStage;
  mEntryPoint = entryPoint;

  // 1. Create shader module.
  std::vector<char> codeBuffer = readFile(fileName);
  createShaderModule(codeBuffer);

  // 2. Shader stage creation.
  // Assign shaders to a specific pipeline stage.
  mShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  mShaderStageInfo.stage = mShaderStage;
  mShaderStageInfo.module = mShaderModule;
  mShaderStageInfo.pName = mEntryPoint.c_str();
}

ShaderStageInfo::~ShaderStageInfo() {
  if (mShaderModule != VK_NULL_HANDLE) {
    vkDestroyShaderModule(mDevice->getDevice(), mShaderModule, nullptr);
  }
}

// Before we can pass the codeBuffer to the pipeline, we have to wrap it in a
// VkShaderModule object.
void ShaderStageInfo::createShaderModule(const std::vector<char> &codeBuffer) {
  VkShaderModuleCreateInfo shaderCreateInfo{};
  shaderCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  shaderCreateInfo.codeSize = codeBuffer.size();
  shaderCreateInfo.pCode =
      reinterpret_cast<const uint32_t *>(codeBuffer.data());

  if (vkCreateShaderModule(mDevice->getDevice(), &shaderCreateInfo, nullptr,
                           &mShaderModule) != VK_SUCCESS) {
    throw std::runtime_error("Error: failed to create shader");
  }
}

GraphicsPipeline::GraphicsPipeline(const Device::Ptr &device,
                                   const RenderPass::Ptr &renderPass) {
  mDevice = device;
  mRenderPass = renderPass;

  mVertexInputInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  mInputAssembly.sType =
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  mViewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  mRasterizer.sType =
      VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  mMultisampling.sType =
      VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  mColorBlending.sType =
      VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  mPipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
}

GraphicsPipeline::~GraphicsPipeline() {
  if (mGraphicsPipeline != VK_NULL_HANDLE) {
    vkDestroyPipeline(mDevice->getDevice(), mGraphicsPipeline, nullptr);
  }

  if (mPipelineLayout != VK_NULL_HANDLE) {
    vkDestroyPipelineLayout(mDevice->getDevice(), mPipelineLayout, nullptr);
  }

  mDevice.reset();
  mRenderPass.reset();
}

void GraphicsPipeline::buildPipeline() {
  // 2.4.Viewports and scissors.
  mViewportState.viewportCount = static_cast<uint32_t>(mViewports.size());
  mViewportState.pViewports = mViewports.data();
  mViewportState.scissorCount = static_cast<uint32_t>(mScissors.size());
  mViewportState.pScissors = mScissors.data();

  // 2.8.Color blending.
  mColorBlending.attachmentCount =
      static_cast<uint32_t>(mBlendAttachmentStates.size());
  mColorBlending.pAttachments = mBlendAttachmentStates.data();

  // 3.Pipeline layout.
  if (mPipelineLayout != VK_NULL_HANDLE) {
    vkDestroyPipelineLayout(mDevice->getDevice(), mPipelineLayout, nullptr);
  }

  if (vkCreatePipelineLayout(mDevice->getDevice(), &mPipelineLayoutInfo,
                             nullptr, &mPipelineLayout) != VK_SUCCESS) {
    throw std::runtime_error("Error: failed to create pipeline layout!");
  }

  // 4.Create graphics pipeline.
  VkGraphicsPipelineCreateInfo pipelineInfo{};
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

  pipelineInfo.stageCount = static_cast<uint32_t>(mShaderStages.size());
  pipelineInfo.pStages = mShaderStages.data();

  pipelineInfo.pVertexInputState = &mVertexInputInfo;
  pipelineInfo.pInputAssemblyState = &mInputAssembly;
  pipelineInfo.pViewportState = &mViewportState;
  pipelineInfo.pRasterizationState = &mRasterizer;
  pipelineInfo.pMultisampleState = &mMultisampling;
  pipelineInfo.pDepthStencilState = nullptr;  // TODO: add depth and stencil
  pipelineInfo.pColorBlendState = &mColorBlending;
  // pipelineInfo.pDynamicState = &dynamicState;
  pipelineInfo.layout = mPipelineLayout;
  pipelineInfo.renderPass = mRenderPass->getRenderPass();
  pipelineInfo.subpass = 0;

  pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;  // Optional
  pipelineInfo.basePipelineIndex = -1;               // Optional

  if (mGraphicsPipeline != VK_NULL_HANDLE) {
    vkDestroyPipeline(mDevice->getDevice(), mGraphicsPipeline, nullptr);
  }

  if (vkCreateGraphicsPipelines(mDevice->getDevice(), VK_NULL_HANDLE, 1,
                                &pipelineInfo, nullptr,
                                &mGraphicsPipeline) != VK_SUCCESS) {
    throw std::runtime_error("Error: failed to create graphics pipeline!");
  }
}
