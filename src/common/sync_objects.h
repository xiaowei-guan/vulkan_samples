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

#ifndef COMMON_SYNC_OBJECTS_H_
#define COMMON_SYNC_OBJECTS_H_

#include <memory>

#include "common/device.h"

class Semaphore {
 public:
  explicit Semaphore(const std::shared_ptr<Device> &device);

  ~Semaphore();

  [[nodiscard]] auto GetSemaphore() const { return vk_semaphore_; }

 private:
  VkSemaphore vk_semaphore_ = VK_NULL_HANDLE;
  std::shared_ptr<Device> device_ = nullptr;
};

class Fence {
 public:
  explicit Fence(const std::shared_ptr<Device> &device, bool signaled = true);

  ~Fence();

  void ResetFence();

  void Block(uint64_t timeout = UINT64_MAX);

  [[nodiscard]] auto GetFence() const { return vk_fence_; }

 private:
  VkFence vk_fence_ = VK_NULL_HANDLE;
  std::shared_ptr<Device> device_ = nullptr;
};

#endif  // COMMON_SYNC_OBJECTS_H_
