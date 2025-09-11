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

#ifndef COMMON_DEVICE_H_
#define COMMON_DEVICE_H_

#include <optional>
#include <memory>
#include <vector>

#include "common/instance.h"
#include "common/window_surface.h"

const std::vector<const char *> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME};

struct QueueFamilyIndices {
  std::optional<uint32_t> graphicsFamily;
  // ensure that the device can present images to the surface we created.
  std::optional<uint32_t> presentFamily;

  bool isComplete() {
    return graphicsFamily.has_value() && presentFamily.has_value();
  }
};

class Device {
 public:
  using Ptr = std::shared_ptr<Device>;
  static Ptr Create(const Instance::Ptr &instance,
                    const WindowSurface::Ptr &windowSurface) {
    return std::make_shared<Device>(instance, windowSurface);
  }

  Device(const Instance::Ptr &instance,
         const WindowSurface::Ptr &windowSurface);

  ~Device();

  void PickPhysicalDevice();

  bool IsDeviceSuitable(VkPhysicalDevice device);

  int RateDeviceSuitability(VkPhysicalDevice device);

  void FindQueueFamilies(VkPhysicalDevice device);

  void CreateLogicalDevice();

  bool CheckDeviceExtensionSupport(VkPhysicalDevice device);

  [[nodiscard]] auto GetDevice() const { return mDevice; }
  [[nodiscard]] auto GetPhysicalDevice() const { return mPhysicalDevice; }
  [[nodiscard]] auto GetQueueFamilyIndices() const {
    return mQueueFamilyIndices;
  }
  [[nodiscard]] auto GetGraphicsQueue() const { return mGraphicsQueue; }
  [[nodiscard]] auto GetPresentQueue() const { return mPresentQueue; }

 private:
  VkPhysicalDevice mPhysicalDevice{VK_NULL_HANDLE};
  Instance::Ptr mInstance{nullptr};
  WindowSurface::Ptr mWindowSurface{nullptr};
  VkDevice mDevice{VK_NULL_HANDLE};  // logical device handle
  QueueFamilyIndices mQueueFamilyIndices{};
  VkQueue mGraphicsQueue{VK_NULL_HANDLE};
  VkQueue mPresentQueue{VK_NULL_HANDLE};
};

#endif  // COMMON_DEVICE_H_
