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

CommandPool::CommandPool(const std::shared_ptr<Device> &device,
                         VkCommandPoolCreateFlagBits flag)
    : device_(device) {
  QueueFamilyIndices queueFamilyIndices = device_->GetQueueFamilyIndices();

  VkCommandPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolInfo.flags = flag;
  poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

  if (vkCreateCommandPool(device_->GetDevice(), &poolInfo, nullptr,
                          &command_pool_) != VK_SUCCESS) {
    throw std::runtime_error("Error: failed to create command pool!");
  }
}

CommandPool::~CommandPool() {
  if (command_pool_ != VK_NULL_HANDLE) {
    vkDestroyCommandPool(device_->GetDevice(), command_pool_, nullptr);
  }
}

CommandBuffer::CommandBuffer(const std::shared_ptr<Device> &device,
                             const std::shared_ptr<CommandPool> &commandPool,
                             bool asSecondary)
    : device_(device), command_pool_(commandPool) {
  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = command_pool_->GetCommandPool();
  allocInfo.level = asSecondary ? VK_COMMAND_BUFFER_LEVEL_SECONDARY
                                : VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = 1;

  if (vkAllocateCommandBuffers(device_->GetDevice(), &allocInfo,
                               &command_buffer_) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate command buffers!");
  }
}

CommandBuffer::~CommandBuffer() {}

void CommandBuffer::Begin(VkCommandBufferUsageFlags flag,
                          const VkCommandBufferInheritanceInfo &inheritance) {
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = flag;
  beginInfo.pInheritanceInfo = &inheritance;

  if (vkBeginCommandBuffer(command_buffer_, &beginInfo) != VK_SUCCESS) {
    throw std::runtime_error("failed to begin recording command buffer!");
  }
}

void CommandBuffer::BeginRenderPass(
    const VkRenderPassBeginInfo &renderPassBeginInfo,
    const VkSubpassContents &subPassContents) {
  vkCmdBeginRenderPass(command_buffer_, &renderPassBeginInfo, subPassContents);
}

void CommandBuffer::BindGraphicPipeline(const VkPipeline &pipeline) {
  vkCmdBindPipeline(command_buffer_, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
}

void CommandBuffer::Draw(size_t vertexCount) {
  vkCmdDraw(command_buffer_, vertexCount, 1, 0, 0);
}

void CommandBuffer::EndRenderPass() { vkCmdEndRenderPass(command_buffer_); }

void CommandBuffer::End() {
  if (vkEndCommandBuffer(command_buffer_) != VK_SUCCESS) {
    throw std::runtime_error("Error:failed to end Command Buffer");
  }
}
