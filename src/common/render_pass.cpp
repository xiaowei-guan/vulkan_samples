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
  mColorAttachmentReferences.push_back(ref);
}

void SubPass::AddInputAttachmentReference(const VkAttachmentReference& ref) {
  mInputAttachmentReferences.push_back(ref);
}

void SubPass::SetDepthStencilAttachmentReference(
    const VkAttachmentReference& ref) {
  mDepthStencilAttachmentReference = ref;
}

void SubPass::BuildSubPassDescription() {
  if (mColorAttachmentReferences.empty()) {
    throw std::runtime_error("Error: color attachment group is empty!");
  }
  mSubPassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

  mSubPassDescription.colorAttachmentCount =
      static_cast<uint32_t>(mColorAttachmentReferences.size());
  mSubPassDescription.pColorAttachments = mColorAttachmentReferences.data();

  mSubPassDescription.inputAttachmentCount =
      static_cast<uint32_t>(mInputAttachmentReferences.size());
  mSubPassDescription.pInputAttachments = mInputAttachmentReferences.data();

  mSubPassDescription.pDepthStencilAttachment =
      mDepthStencilAttachmentReference.layout == VK_IMAGE_LAYOUT_UNDEFINED
          ? nullptr
          : &mDepthStencilAttachmentReference;
}

RenderPass::RenderPass(const Device::Ptr& device) { mDevice = device; }

RenderPass::~RenderPass() {
  if (mRenderPass != VK_NULL_HANDLE) {
    vkDestroyRenderPass(mDevice->GetDevice(), mRenderPass, nullptr);
  }
}

void RenderPass::AddSubPass(const SubPass& subpass) {
  mSubPasses.push_back(subpass);
}

void RenderPass::AddDependency(const VkSubpassDependency& dependency) {
  mDependencies.push_back(dependency);
}

void RenderPass::AddAttachment(const VkAttachmentDescription& attachmentDes) {
  mAttachmentDescriptions.push_back(attachmentDes);
}

void RenderPass::BuildRenderPass() {
  if (mSubPasses.empty() || mAttachmentDescriptions.empty() ||
      mDependencies.empty()) {
    throw std::runtime_error("Error: not enough elements to build renderPass");
  }

  // unwrap
  std::vector<VkSubpassDescription> subPasses{};
  for (int i = 0; i < mSubPasses.size(); ++i) {
    subPasses.push_back(mSubPasses[i].GetSubPassDescription());
  }

  VkRenderPassCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;

  createInfo.attachmentCount =
      static_cast<uint32_t>(mAttachmentDescriptions.size());
  createInfo.pAttachments = mAttachmentDescriptions.data();

  createInfo.dependencyCount = static_cast<uint32_t>(mDependencies.size());
  createInfo.pDependencies = mDependencies.data();

  createInfo.subpassCount = static_cast<uint32_t>(subPasses.size());
  createInfo.pSubpasses = subPasses.data();

  if (vkCreateRenderPass(mDevice->GetDevice(), &createInfo, nullptr,
                         &mRenderPass) != VK_SUCCESS) {
    throw std::runtime_error("Error: failed to create renderPass");
  }
}
