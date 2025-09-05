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

FrameBuffers::FrameBuffers(const Device::Ptr &device,
                           const SwapChain::Ptr &swapChain,
                           const RenderPass::Ptr &renderPass) {
  mDevice = device;
  mSwapChain = swapChain;
  mRenderPass = renderPass;

  size_t imageCount = mSwapChain->getSwapChainImageCount();
  mSwapChainFramebuffers.resize(imageCount);

  for (size_t i = 0; i < imageCount; ++i) {
    VkImageView attachments[] = {mSwapChain->getSwapChainImageView(i)};

    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = mRenderPass->getRenderPass();
    framebufferInfo.attachmentCount = 1;
    framebufferInfo.pAttachments = attachments;
    framebufferInfo.width = mSwapChain->getSwapChainExtent().width;
    framebufferInfo.height = mSwapChain->getSwapChainExtent().height;
    framebufferInfo.layers = 1;

    if (vkCreateFramebuffer(mDevice->getDevice(), &framebufferInfo, nullptr,
                            &mSwapChainFramebuffers[i]) != VK_SUCCESS) {
      throw std::runtime_error("Error: failed to create framebuffer!");
    }
  }
}

FrameBuffers::~FrameBuffers() {
  for (auto framebuffer : mSwapChainFramebuffers) {
    vkDestroyFramebuffer(mDevice->getDevice(), framebuffer, nullptr);
  }

  mDevice.reset();
  mSwapChain.reset();
  mRenderPass.reset();
}
