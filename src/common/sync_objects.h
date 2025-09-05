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
  using Ptr = std::shared_ptr<Semaphore>;
  static Ptr create(const Device::Ptr &device) {
    return std::make_shared<Semaphore>(device);
  }

  explicit Semaphore(const Device::Ptr &device);

  ~Semaphore();

  [[nodiscard]] auto getSemaphore() const { return mSemaphore; }

 private:
  VkSemaphore mSemaphore{VK_NULL_HANDLE};
  Device::Ptr mDevice{nullptr};
};

class Fence {
 public:
  using Ptr = std::shared_ptr<Fence>;
  static Ptr create(const Device::Ptr &device, bool signaled = true) {
    return std::make_shared<Fence>(device, signaled);
  }

  explicit Fence(const Device::Ptr &device, bool signaled = true);

  ~Fence();

  void resetFence();

  void block(uint64_t timeout = UINT64_MAX);

  [[nodiscard]] auto getFence() const { return mFence; }

 private:
  VkFence mFence{VK_NULL_HANDLE};
  Device::Ptr mDevice{nullptr};
};

#endif  // COMMON_SYNC_OBJECTS_H_
