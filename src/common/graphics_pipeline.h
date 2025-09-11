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
  using Ptr = std::shared_ptr<ShaderStageInfo>;
  static Ptr Create(const Device::Ptr &device, const std::string &fileName,
                    VkShaderStageFlagBits shaderStage,
                    const std::string &entryPoint) {
    return std::make_shared<ShaderStageInfo>(device, fileName, shaderStage,
                                             entryPoint);
  }

  ShaderStageInfo(const Device::Ptr &device, const std::string &fileName,
                  VkShaderStageFlagBits shaderStage,
                  const std::string &entryPoint);

  ~ShaderStageInfo();

  [[nodiscard]] auto GetShaderStage() const { return mShaderStage; }
  [[nodiscard]] auto &GetShaderEntryPoint() const { return mEntryPoint; }
  [[nodiscard]] auto GetShaderModule() const { return mShaderModule; }
  [[nodiscard]] auto GetShaderStageInfo() const { return mShaderStageInfo; }

 private:
  void CreateShaderModule(const std::vector<char> &code);

 private:
  VkShaderModule mShaderModule{VK_NULL_HANDLE};
  VkPipelineShaderStageCreateInfo mShaderStageInfo{};

  Device::Ptr mDevice{nullptr};
  std::string mEntryPoint;

  VkShaderStageFlagBits mShaderStage;
};

class GraphicsPipeline {
 public:
  using Ptr = std::shared_ptr<GraphicsPipeline>;
  static Ptr Create(const Device::Ptr &device,
                    const RenderPass::Ptr &renderPass) {
    return std::make_shared<GraphicsPipeline>(device, renderPass);
  }

  GraphicsPipeline(const Device::Ptr &device,
                   const RenderPass::Ptr &renderPass);
  ~GraphicsPipeline();

  void SetShaderStages(
      const std::vector<VkPipelineShaderStageCreateInfo> &shaderStages) {
    mShaderStages = shaderStages;
  }

  void SetViewports(const std::vector<VkViewport> &viewports) {
    mViewports = viewports;
  }

  void SetScissors(const std::vector<VkRect2D> &scissors) {
    mScissors = scissors;
  }

  void AddBlendAttachment(
      const VkPipelineColorBlendAttachmentState &blendAttachment) {
    mBlendAttachmentStates.push_back(blendAttachment);
  }

  void BuildPipeline();

  [[nodiscard]] auto GetPipelineLayout() const { return mPipelineLayout; }
  [[nodiscard]] auto GetPipeline() const { return mGraphicsPipeline; }

 public:
  VkPipelineVertexInputStateCreateInfo mVertexInputInfo{};
  VkPipelineInputAssemblyStateCreateInfo mInputAssembly{};
  VkPipelineViewportStateCreateInfo mViewportState{};
  VkPipelineRasterizationStateCreateInfo mRasterizer{};
  VkPipelineMultisampleStateCreateInfo mMultisampling{};

  std::vector<VkPipelineColorBlendAttachmentState> mBlendAttachmentStates{};
  VkPipelineColorBlendStateCreateInfo mColorBlending{};

  VkPipelineLayoutCreateInfo mPipelineLayoutInfo{};

 private:
  VkPipelineLayout mPipelineLayout{VK_NULL_HANDLE};
  VkPipeline mGraphicsPipeline{VK_NULL_HANDLE};

  Device::Ptr mDevice{nullptr};
  RenderPass::Ptr mRenderPass{nullptr};

  std::vector<VkPipelineShaderStageCreateInfo> mShaderStages;
  std::vector<VkViewport> mViewports{};
  std::vector<VkRect2D> mScissors{};
};

#endif  // COMMON_GRAPHICS_PIPELINE_H_
