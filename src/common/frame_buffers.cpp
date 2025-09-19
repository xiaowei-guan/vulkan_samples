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

#include "common/frame_buffers.h"

FrameBuffers::FrameBuffers(const std::shared_ptr<Device> &device,
                           const std::shared_ptr<SwapChain> &swapChain,
                           const std::shared_ptr<RenderPass> &renderPass)
    : device_(device), swap_chain_(swapChain), render_pass_(renderPass) {
  size_t imageCount = swap_chain_->GetSwapChainImageCount();
  swap_chain_frame_buffers_.resize(imageCount);

  for (size_t i = 0; i < imageCount; ++i) {
    VkImageView attachments[] = {swap_chain_->GetSwapChainImageView(i)};

    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = render_pass_->GetRenderPass();
    framebufferInfo.attachmentCount = 1;
    framebufferInfo.pAttachments = attachments;
    framebufferInfo.width = swap_chain_->GetSwapChainExtent().width;
    framebufferInfo.height = swap_chain_->GetSwapChainExtent().height;
    framebufferInfo.layers = 1;

    if (vkCreateFramebuffer(device_->GetDevice(), &framebufferInfo, nullptr,
                            &swap_chain_frame_buffers_[i]) != VK_SUCCESS) {
      throw std::runtime_error("Error: failed to create framebuffer!");
    }
  }
}

FrameBuffers::~FrameBuffers() {
  for (auto framebuffer : swap_chain_frame_buffers_) {
    vkDestroyFramebuffer(device_->GetDevice(), framebuffer, nullptr);
  }
}
