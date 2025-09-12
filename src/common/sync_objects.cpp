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

#include "common/sync_objects.h"

Semaphore::Semaphore(const std::shared_ptr<Device> &device) {
  device_ = device;

  VkSemaphoreCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  if (vkCreateSemaphore(device_->GetDevice(), &createInfo, nullptr,
                        &vk_semaphore_) != VK_SUCCESS) {
    throw std::runtime_error("Error: failed to create Semaphore");
  }
}

Semaphore::~Semaphore() {
  if (vk_semaphore_ != VK_NULL_HANDLE) {
    vkDestroySemaphore(device_->GetDevice(), vk_semaphore_, nullptr);
  }
}

Fence::Fence(const std::shared_ptr<Device> &device, bool signaled) {
  device_ = device;

  VkFenceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  createInfo.flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

  if (vkCreateFence(device_->GetDevice(), &createInfo, nullptr, &vk_fence_) !=
      VK_SUCCESS) {
    throw std::runtime_error("Error:failed to create fence");
  }
}

Fence::~Fence() {
  if (vk_fence_ != VK_NULL_HANDLE) {
    vkDestroyFence(device_->GetDevice(), vk_fence_, nullptr);
  }
}

void Fence::ResetFence() { vkResetFences(device_->GetDevice(), 1, &vk_fence_); }

void Fence::Block(uint64_t timeout) {
  vkWaitForFences(device_->GetDevice(), 1, &vk_fence_, VK_TRUE, timeout);
}
