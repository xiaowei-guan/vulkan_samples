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

#ifndef COMMON_SWAP_CHAIN_H_
#define COMMON_SWAP_CHAIN_H_

#include <memory>
#include <vector>

#include "common/device.h"
#include "common/window.h"
#include "common/window_surface.h"

struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;
};

class SwapChain {
 public:
  SwapChain(const std::shared_ptr<Device> &device,
            const std::shared_ptr<WindowSurface> &surface,
            const std::shared_ptr<Window> &window);

  ~SwapChain();

  SwapChainSupportDetails QuerySwapChainSupport();

  VkSurfaceFormatKHR ChooseSwapSurfaceFormat(
      const std::vector<VkSurfaceFormatKHR> &availableFormats);

  VkPresentModeKHR ChooseSwapPresentMode(
      const std::vector<VkPresentModeKHR> &availablePresentModes);

  VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

  [[nodiscard]] auto GetSwapChainExtent() const { return swapchain_extent_; }

  [[nodiscard]] auto GetSwapChainImageFormat() const {
    return swapchain_image_format_;
  }

  [[nodiscard]] auto GetSwapChainImageCount() const { return image_count_; }

  [[nodiscard]] auto GetSwapChainImageView(const int index) const {
    return swapchain_imege_views_[index];
  }

  [[nodiscard]] auto GetSwapChain() const { return vk_swapchain_; }

 private:
  void CreateImageViews();

 private:
  std::shared_ptr<Device> device_ = nullptr;
  std::shared_ptr<WindowSurface> window_surface_ = nullptr;
  VkSwapchainKHR vk_swapchain_ = VK_NULL_HANDLE;
  std::shared_ptr<Window> window_;
  std::vector<VkImage> swapchain_images_;
  VkFormat swapchain_image_format_;
  VkExtent2D swapchain_extent_ ;
  uint32_t image_count_ = 0;

  std::vector<VkImageView> swapchain_imege_views_;
};

#endif  // COMMON_SWAP_CHAIN_H_
