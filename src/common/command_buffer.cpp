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

#include "common/command_buffer.h"

#include <iostream>
#include <stdexcept>

CommandPool::CommandPool(const Device::Ptr &device,
                         VkCommandPoolCreateFlagBits flag) {
  mDevice = device;

  QueueFamilyIndices queueFamilyIndices = mDevice->getQueueFamilyIndices();

  VkCommandPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolInfo.flags = flag;
  poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

  if (vkCreateCommandPool(mDevice->getDevice(), &poolInfo, nullptr,
                          &mCommandPool) != VK_SUCCESS) {
    throw std::runtime_error("Error: failed to create command pool!");
  }
}

CommandPool::~CommandPool() {
  if (mCommandPool != VK_NULL_HANDLE) {
    vkDestroyCommandPool(mDevice->getDevice(), mCommandPool, nullptr);
  }
}

CommandBuffer::CommandBuffer(const Device::Ptr &device,
                             const CommandPool::Ptr &commandPool,
                             bool asSecondary) {
  mDevice = device;
  mCommandPool = commandPool;

  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = mCommandPool->getCommandPool();
  allocInfo.level = asSecondary ? VK_COMMAND_BUFFER_LEVEL_SECONDARY
                                : VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = 1;

  if (vkAllocateCommandBuffers(mDevice->getDevice(), &allocInfo,
                               &mCommandBuffer) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate command buffers!");
  }
}

CommandBuffer::~CommandBuffer() {
  mCommandPool.reset();
  mDevice.reset();
}

void CommandBuffer::begin(VkCommandBufferUsageFlags flag,
                          const VkCommandBufferInheritanceInfo &inheritance) {
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = flag;
  beginInfo.pInheritanceInfo = &inheritance;

  if (vkBeginCommandBuffer(mCommandBuffer, &beginInfo) != VK_SUCCESS) {
    throw std::runtime_error("failed to begin recording command buffer!");
  }
}

void CommandBuffer::beginRenderPass(
    const VkRenderPassBeginInfo &renderPassBeginInfo,
    const VkSubpassContents &subPassContents) {
  vkCmdBeginRenderPass(mCommandBuffer, &renderPassBeginInfo, subPassContents);
}

void CommandBuffer::bindGraphicPipeline(const VkPipeline &pipeline) {
  vkCmdBindPipeline(mCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
}

void CommandBuffer::draw(size_t vertexCount) {
  vkCmdDraw(mCommandBuffer, vertexCount, 1, 0, 0);
}

void CommandBuffer::endRenderPass() { vkCmdEndRenderPass(mCommandBuffer); }

void CommandBuffer::end() {
  if (vkEndCommandBuffer(mCommandBuffer) != VK_SUCCESS) {
    throw std::runtime_error("Error:failed to end Command Buffer");
  }
}
