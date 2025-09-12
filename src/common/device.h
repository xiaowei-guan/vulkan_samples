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

#include <memory>
#include <optional>
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
  Device(const std::shared_ptr<Instance> &instance,
         const std::shared_ptr<WindowSurface> &windowSurface);

  ~Device();

  void PickPhysicalDevice();

  bool IsDeviceSuitable(VkPhysicalDevice device);

  int RateDeviceSuitability(VkPhysicalDevice device);

  void FindQueueFamilies(VkPhysicalDevice device);

  void CreateLogicalDevice();

  bool CheckDeviceExtensionSupport(VkPhysicalDevice device);

  [[nodiscard]] auto GetDevice() const { return device_; }
  [[nodiscard]] auto GetPhysicalDevice() const { return physical_device_; }
  [[nodiscard]] auto GetQueueFamilyIndices() const {
    return queue_family_indices_;
  }
  [[nodiscard]] auto GetGraphicsQueue() const { return graphics_queue_; }
  [[nodiscard]] auto GetPresentQueue() const { return present_queue_; }

 private:
  VkPhysicalDevice physical_device_ = VK_NULL_HANDLE;
  std::shared_ptr<Instance> instance_ = nullptr;
  std::shared_ptr<WindowSurface> window_surface_ = nullptr;
  VkDevice device_ = VK_NULL_HANDLE;  // logical device handle
  QueueFamilyIndices queue_family_indices_;
  VkQueue graphics_queue_ = VK_NULL_HANDLE;
  VkQueue present_queue_ = VK_NULL_HANDLE;
};

#endif  // COMMON_DEVICE_H_
