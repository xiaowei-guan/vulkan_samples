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

#ifndef COMMAND_BUFFER_H_
#define COMMAND_BUFFER_H_

#include <memory>

#include "common/device.h"

class CommandPool {
 public:
  using Ptr = std::shared_ptr<CommandPool>;
  static Ptr Create(const Device::Ptr &device,
                    VkCommandPoolCreateFlagBits flag =
                        VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT) {
    return std::make_shared<CommandPool>(device, flag);
  }

  CommandPool(const Device::Ptr &device,
              VkCommandPoolCreateFlagBits flag =
                  VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
  ~CommandPool();

  [[nodiscard]] auto GetCommandPool() const { return mCommandPool; }

 private:
  VkCommandPool mCommandPool{VK_NULL_HANDLE};

  Device::Ptr mDevice{nullptr};
};

class CommandBuffer {
 public:
  using Ptr = std::shared_ptr<CommandBuffer>;
  static Ptr Create(const Device::Ptr &device,
                    const CommandPool::Ptr &commandPool,
                    bool asSecondary = false) {
    return std::make_shared<CommandBuffer>(device, commandPool, asSecondary);
  }

  CommandBuffer(const Device::Ptr &device, const CommandPool::Ptr &commandPool,
                bool asSecondary = false);
  ~CommandBuffer();

  void Begin(VkCommandBufferUsageFlags flag = 0,
             const VkCommandBufferInheritanceInfo &inheritance = {
                 .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO});
  void BeginRenderPass(
      const VkRenderPassBeginInfo &renderPassBeginInfo,
      const VkSubpassContents &subPassContents = VK_SUBPASS_CONTENTS_INLINE);
  void BindGraphicPipeline(const VkPipeline &pipeline);
  void Draw(size_t vertexCount);
  void EndRenderPass();
  void End();

  [[nodiscard]] auto GetCommandBuffer() const { return mCommandBuffer; }

 private:
  VkCommandBuffer mCommandBuffer{VK_NULL_HANDLE};

  Device::Ptr mDevice{nullptr};
  CommandPool::Ptr mCommandPool{nullptr};
};

#endif  // COMMAND_BUFFER_H_
