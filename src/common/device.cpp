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

#include "common/device.h"

#include <iostream>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

// Validation layers.
const std::vector<const char *> validationLayers = {
    "VK_LAYER_KHRONOS_validation"};

Device::Device(const std::shared_ptr<Instance> &instance,
               const std::shared_ptr<WindowSurface> &windowSurface)
    : instance_(instance), window_surface_(windowSurface) {
  PickPhysicalDevice();
  FindQueueFamilies(physical_device_);
  CreateLogicalDevice();
}

Device::~Device() { vkDestroyDevice(device_, nullptr); }

void Device::PickPhysicalDevice() {
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(instance_->GetInstance(), &deviceCount, nullptr);
  if (deviceCount == 0) {
    throw std::runtime_error("Error:failed to emumerate physical devices!");
  }

  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(instance_->GetInstance(), &deviceCount,
                             devices.data());

  // Use an ordered map to automatically sort candidates by increasing score
  std::multimap<int, VkPhysicalDevice> candidates;

  for (const auto &device : devices) {
    int score = RateDeviceSuitability(device);
    candidates.insert(std::make_pair(score, device));
  }

  // Check if the best candidate is suitable at all
  if (candidates.rbegin()->first > 0 &&
      IsDeviceSuitable(candidates.rbegin()->second)) {
    physical_device_ = candidates.rbegin()->second;
  }

  if (physical_device_ == VK_NULL_HANDLE) {
    throw std::runtime_error("Error: failed to find a suitable GPU!");
  }
}

// Support for swapChain.  Now.
bool Device::CheckDeviceExtensionSupport(VkPhysicalDevice device) {
  uint32_t extensionCount;
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount,
                                       nullptr);

  std::vector<VkExtensionProperties> availableExtensions(extensionCount);
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount,
                                       availableExtensions.data());

  std::set<std::string> requiredExtensions(deviceExtensions.begin(),
                                           deviceExtensions.end());

  for (const auto &extension : availableExtensions) {
    requiredExtensions.erase(extension.extensionName);
  }

  return requiredExtensions.empty();
}

bool Device::IsDeviceSuitable(VkPhysicalDevice device) {
  // Base devices suitability check.
  VkPhysicalDeviceProperties deviceProperties;
  VkPhysicalDeviceFeatures deviceFeatures;
  vkGetPhysicalDeviceProperties(device, &deviceProperties);
  vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

  return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
         deviceFeatures.geometryShader;
}

// Give each deivce a score and pick the highest one.
int Device::RateDeviceSuitability(VkPhysicalDevice device) {
  int score = 0;
  VkPhysicalDeviceProperties deviceProperties;
  VkPhysicalDeviceFeatures deviceFeatures;
  vkGetPhysicalDeviceProperties(device, &deviceProperties);
  vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

  if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
    score += 1000;
  }

  score += deviceProperties.limits.maxImageDimension2D;

  if (!deviceFeatures.geometryShader) {
    return 0;
  }

  return score;
}

// Anything from drawing to uploading textures, requires commands to be
// submitted to a queue. So now querying the available queue families.   After
// that, specify which queues to create.
void Device::FindQueueFamilies(VkPhysicalDevice device) {
  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
                                           queueFamilies.data());

  int i = 0;
  for (const auto &queueFamily : queueFamilies) {
    if (queueFamily.queueCount > 0 &&
        (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
      queue_family_indices_.graphicsFamily = i;
    }

    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(
        device, i, window_surface_->GetSurface(), &presentSupport);
    if (presentSupport) {
      queue_family_indices_.presentFamily = i;
    }

    if (queue_family_indices_.isComplete()) {
      break;
    }

    i++;
  }
}

void Device::CreateLogicalDevice() {
  // Specifying the queues to be created.
  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
  std::set<uint32_t> uniqueQueueFamilies = {
      queue_family_indices_.graphicsFamily.value(),
      queue_family_indices_.presentFamily.value()};

  float queuePriority = 1.0f;
  for (uint32_t queueFamily : uniqueQueueFamilies) {
    VkDeviceQueueCreateInfo queueCreateInfo = {};

    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = queueFamily;
    // Don't really need more than one queue.
    // Because you can create all of the command buffers on multiple threads
    // and then submit them all at once on the main thread.
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    queueCreateInfos.push_back(queueCreateInfo);
  }

  // Creating the logical device.
  VkDeviceCreateInfo createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  createInfo.pQueueCreateInfos = queueCreateInfos.data();
  createInfo.queueCreateInfoCount =
      static_cast<uint32_t>(queueCreateInfos.size());

  VkPhysicalDeviceFeatures deviceFeatures = {};
  createInfo.pEnabledFeatures = &deviceFeatures;

  // Enable swapChain extension.  Now.
  createInfo.enabledExtensionCount =
      static_cast<uint32_t>(deviceExtensions.size());
  createInfo.ppEnabledExtensionNames = deviceExtensions.data();

  if (instance_->IsValidationLayerEnabled()) {
    createInfo.enabledLayerCount =
        static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames = validationLayers.data();
  } else {
    createInfo.enabledLayerCount = 0;
  }

  if (vkCreateDevice(physical_device_, &createInfo, nullptr, &device_) !=
      VK_SUCCESS) {
    throw std::runtime_error("Error: failed to create logical device!");
  }

  // Retrieving queue handles.
  // Device queues are implicitly cleaned up when the device is destroyed.
  vkGetDeviceQueue(device_, queue_family_indices_.graphicsFamily.value(), 0,
                   &graphics_queue_);
  vkGetDeviceQueue(device_, queue_family_indices_.presentFamily.value(), 0,
                   &present_queue_);
}
