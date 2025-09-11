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

Semaphore::Semaphore(const Device::Ptr &device) {
  mDevice = device;

  VkSemaphoreCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  if (vkCreateSemaphore(mDevice->GetDevice(), &createInfo, nullptr,
                        &mSemaphore) != VK_SUCCESS) {
    throw std::runtime_error("Error: failed to create Semaphore");
  }
}

Semaphore::~Semaphore() {
  if (mSemaphore != VK_NULL_HANDLE) {
    vkDestroySemaphore(mDevice->GetDevice(), mSemaphore, nullptr);
  }
}

Fence::Fence(const Device::Ptr &device, bool signaled) {
  mDevice = device;

  VkFenceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  createInfo.flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

  if (vkCreateFence(mDevice->GetDevice(), &createInfo, nullptr, &mFence) !=
      VK_SUCCESS) {
    throw std::runtime_error("Error:failed to create fence");
  }
}

Fence::~Fence() {
  if (mFence != VK_NULL_HANDLE) {
    vkDestroyFence(mDevice->GetDevice(), mFence, nullptr);
  }
}

void Fence::resetFence() { vkResetFences(mDevice->GetDevice(), 1, &mFence); }

void Fence::Block(uint64_t timeout) {
  vkWaitForFences(mDevice->GetDevice(), 1, &mFence, VK_TRUE, timeout);
}
