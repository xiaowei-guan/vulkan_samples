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

#include "common/render_pass.h"

#include <stdexcept>

SubPass::~SubPass() {}

void SubPass::AddColorAttachmentReference(const VkAttachmentReference& ref) {
  color_attachment_references_.push_back(ref);
}

void SubPass::AddInputAttachmentReference(const VkAttachmentReference& ref) {
  input_attachment_references_.push_back(ref);
}

void SubPass::SetDepthStencilAttachmentReference(
    const VkAttachmentReference& ref) {
  depth_stencil_attachment_reference_ = ref;
}

void SubPass::BuildSubPassDescription() {
  if (color_attachment_references_.empty()) {
    throw std::runtime_error("Error: color attachment group is empty!");
  }
  vk_subpass_description_.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

  vk_subpass_description_.colorAttachmentCount =
      static_cast<uint32_t>(color_attachment_references_.size());
  vk_subpass_description_.pColorAttachments = color_attachment_references_.data();

  vk_subpass_description_.inputAttachmentCount =
      static_cast<uint32_t>(input_attachment_references_.size());
  vk_subpass_description_.pInputAttachments = input_attachment_references_.data();

  vk_subpass_description_.pDepthStencilAttachment =
      depth_stencil_attachment_reference_.layout == VK_IMAGE_LAYOUT_UNDEFINED
          ? nullptr
          : &depth_stencil_attachment_reference_;
}

RenderPass::RenderPass(const std::shared_ptr<Device>& device) {
  device_ = device;
}

RenderPass::~RenderPass() {
  if (vk_render_pass_ != VK_NULL_HANDLE) {
    vkDestroyRenderPass(device_->GetDevice(), vk_render_pass_, nullptr);
  }
}

void RenderPass::AddSubPass(const SubPass& subpass) {
  subpass_.push_back(subpass);
}

void RenderPass::AddDependency(const VkSubpassDependency& dependency) {
  vk_subpass_dependencies_.push_back(dependency);
}

void RenderPass::AddAttachment(const VkAttachmentDescription& attachmentDes) {
  vk_attachment_descriptions_.push_back(attachmentDes);
}

void RenderPass::BuildRenderPass() {
  if (subpass_.empty() || vk_attachment_descriptions_.empty() ||
      vk_subpass_dependencies_.empty()) {
    throw std::runtime_error("Error: not enough elements to build renderPass");
  }

  // unwrap
  std::vector<VkSubpassDescription> subPasses{};
  for (int i = 0; i < subpass_.size(); ++i) {
    subPasses.push_back(subpass_[i].GetSubPassDescription());
  }

  VkRenderPassCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;

  createInfo.attachmentCount =
      static_cast<uint32_t>(vk_attachment_descriptions_.size());
  createInfo.pAttachments = vk_attachment_descriptions_.data();

  createInfo.dependencyCount = static_cast<uint32_t>(vk_subpass_dependencies_.size());
  createInfo.pDependencies = vk_subpass_dependencies_.data();

  createInfo.subpassCount = static_cast<uint32_t>(subPasses.size());
  createInfo.pSubpasses = subPasses.data();

  if (vkCreateRenderPass(device_->GetDevice(), &createInfo, nullptr,
                         &vk_render_pass_) != VK_SUCCESS) {
    throw std::runtime_error("Error: failed to create renderPass");
  }
}
