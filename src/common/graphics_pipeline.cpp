////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License.  You may obtain a copy
// of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
// License for the specific language governing permissions and limitations
// under the License.
////////////////////////////////////////////////////////////////////////////////

#include "common/graphics_pipeline.h"

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
ShaderStageInfo::ShaderStageInfo(const std::shared_ptr<Device> &device,
                                 const std::string &fileName,
                                 VkShaderStageFlagBits shaderStage,
                                 const std::string &entryPoint) {
  device_ = device;
  shader_stage_ = shaderStage;
  entry_point_ = entryPoint;

  // 1. Create shader module.
  std::vector<char> codeBuffer = readFile(fileName);
  CreateShaderModule(codeBuffer);

  // 2. Shader stage creation.
  // Assign shaders to a specific pipeline stage.
  shader_stage_info_.sType =
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  shader_stage_info_.stage = shader_stage_;
  shader_stage_info_.module = shader_module_;
  shader_stage_info_.pName = entry_point_.c_str();
}

ShaderStageInfo::~ShaderStageInfo() {
  if (shader_module_ != VK_NULL_HANDLE) {
    vkDestroyShaderModule(device_->GetDevice(), shader_module_, nullptr);
  }
}

// Before we can pass the codeBuffer to the pipeline, we have to wrap it in a
// VkShaderModule object.
void ShaderStageInfo::CreateShaderModule(const std::vector<char> &codeBuffer) {
  VkShaderModuleCreateInfo shaderCreateInfo{};
  shaderCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  shaderCreateInfo.codeSize = codeBuffer.size();
  shaderCreateInfo.pCode =
      reinterpret_cast<const uint32_t *>(codeBuffer.data());

  if (vkCreateShaderModule(device_->GetDevice(), &shaderCreateInfo, nullptr,
                           &shader_module_) != VK_SUCCESS) {
    throw std::runtime_error("Error: failed to create shader");
  }
}

GraphicsPipeline::GraphicsPipeline(
    const std::shared_ptr<Device> &device,
    const std::shared_ptr<RenderPass> &renderPass) {
  device_ = device;
  render_pass_ = renderPass;

  vertex_input_info.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  input_assembly.sType =
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  multisampling.sType =
      VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  color_blending.sType =
      VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
}

GraphicsPipeline::~GraphicsPipeline() {
  if (graphics_pipeline_ != VK_NULL_HANDLE) {
    vkDestroyPipeline(device_->GetDevice(), graphics_pipeline_, nullptr);
  }

  if (pipeline_layout != VK_NULL_HANDLE) {
    vkDestroyPipelineLayout(device_->GetDevice(), pipeline_layout, nullptr);
  }

  device_.reset();
  render_pass_.reset();
}

void GraphicsPipeline::BuildPipeline() {
  // 2.4.Viewports and scissors.
  viewport_state.viewportCount = static_cast<uint32_t>(viewports_.size());
  viewport_state.pViewports = viewports_.data();
  viewport_state.scissorCount = static_cast<uint32_t>(scissors_.size());
  viewport_state.pScissors = scissors_.data();

  // 2.8.Color blending.
  color_blending.attachmentCount =
      static_cast<uint32_t>(blend_attachment_states.size());
  color_blending.pAttachments = blend_attachment_states.data();

  // 3.Pipeline layout.
  if (pipeline_layout != VK_NULL_HANDLE) {
    vkDestroyPipelineLayout(device_->GetDevice(), pipeline_layout, nullptr);
  }

  if (vkCreatePipelineLayout(device_->GetDevice(), &pipeline_layout_info,
                             nullptr, &pipeline_layout) != VK_SUCCESS) {
    throw std::runtime_error("Error: failed to create pipeline layout!");
  }

  // 4.Create graphics pipeline.
  VkGraphicsPipelineCreateInfo pipelineInfo{};
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

  pipelineInfo.stageCount = static_cast<uint32_t>(shader_stages_.size());
  pipelineInfo.pStages = shader_stages_.data();

  pipelineInfo.pVertexInputState = &vertex_input_info;
  pipelineInfo.pInputAssemblyState = &input_assembly;
  pipelineInfo.pViewportState = &viewport_state;
  pipelineInfo.pRasterizationState = &rasterizer;
  pipelineInfo.pMultisampleState = &multisampling;
  pipelineInfo.pDepthStencilState = nullptr;
  pipelineInfo.pColorBlendState = &color_blending;
  // pipelineInfo.pDynamicState = &dynamicState;
  pipelineInfo.layout = pipeline_layout;
  pipelineInfo.renderPass = render_pass_->GetRenderPass();
  pipelineInfo.subpass = 0;

  pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;  // Optional
  pipelineInfo.basePipelineIndex = -1;               // Optional

  if (graphics_pipeline_ != VK_NULL_HANDLE) {
    vkDestroyPipeline(device_->GetDevice(), graphics_pipeline_, nullptr);
  }

  if (vkCreateGraphicsPipelines(device_->GetDevice(), VK_NULL_HANDLE, 1,
                                &pipelineInfo, nullptr,
                                &graphics_pipeline_) != VK_SUCCESS) {
    throw std::runtime_error("Error: failed to create graphics pipeline!");
  }
}
