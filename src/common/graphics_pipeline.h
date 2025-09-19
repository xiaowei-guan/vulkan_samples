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

#ifndef COMMON_GRAPHICS_PIPELINE_H_
#define COMMON_GRAPHICS_PIPELINE_H_

#include <memory>
#include <string>
#include <vector>

#include "common/device.h"
#include "common/render_pass.h"

class ShaderStageInfo {
 public:
  ShaderStageInfo(const std::shared_ptr<Device> &device,
                  const std::string &fileName,
                  VkShaderStageFlagBits shaderStage,
                  const std::string &entryPoint);

  ~ShaderStageInfo();

  [[nodiscard]] auto GetShaderStage() const { return shader_stage_; }
  [[nodiscard]] auto &GetShaderEntryPoint() const { return entry_point_; }
  [[nodiscard]] auto GetShaderModule() const { return shader_module_; }
  [[nodiscard]] auto GetShaderStageInfo() const { return shader_stage_info_; }

 private:
  void CreateShaderModule(const std::vector<char> &code);

 private:
  VkShaderModule shader_module_ = VK_NULL_HANDLE;
  VkPipelineShaderStageCreateInfo shader_stage_info_{};
  std::shared_ptr<Device> device_ = nullptr;
  std::string entry_point_;
  VkShaderStageFlagBits shader_stage_;
};

class GraphicsPipeline {
 public:
  GraphicsPipeline(const std::shared_ptr<Device> &device,
                   const std::shared_ptr<RenderPass> &renderPass);
  ~GraphicsPipeline();

  void SetShaderStages(
      const std::vector<VkPipelineShaderStageCreateInfo> &shaderStages) {
    shader_stages_ = shaderStages;
  }

  void SetViewports(const std::vector<VkViewport> &viewports) {
    viewports_ = viewports;
  }

  void SetScissors(const std::vector<VkRect2D> &scissors) {
    scissors_ = scissors;
  }

  void AddBlendAttachment(
      const VkPipelineColorBlendAttachmentState &blendAttachment) {
    blend_attachment_states.push_back(blendAttachment);
  }

  void BuildPipeline();

  [[nodiscard]] auto GetPipelineLayout() const { return pipeline_layout; }
  [[nodiscard]] auto GetPipeline() const { return graphics_pipeline_; }

 public:
  VkPipelineVertexInputStateCreateInfo vertex_input_info{};
  VkPipelineInputAssemblyStateCreateInfo input_assembly{};
  VkPipelineViewportStateCreateInfo viewport_state{};
  VkPipelineRasterizationStateCreateInfo rasterizer{};
  VkPipelineMultisampleStateCreateInfo multisampling{};
  std::vector<VkPipelineColorBlendAttachmentState> blend_attachment_states{};
  VkPipelineColorBlendStateCreateInfo color_blending{};
  VkPipelineLayoutCreateInfo pipeline_layout_info{};

 private:
  VkPipelineLayout pipeline_layout = VK_NULL_HANDLE;
  VkPipeline graphics_pipeline_ = VK_NULL_HANDLE;

  std::shared_ptr<Device> device_ = nullptr;
  std::shared_ptr<RenderPass> render_pass_ = nullptr;

  std::vector<VkPipelineShaderStageCreateInfo> shader_stages_{};
  std::vector<VkViewport> viewports_{};
  std::vector<VkRect2D> scissors_{};
};

#endif  // COMMON_GRAPHICS_PIPELINE_H_
