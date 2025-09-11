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

#include <vector>
#include <memory>

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
  using Ptr = std::shared_ptr<SwapChain>;
  static Ptr Create(const Device::Ptr &device,
                    const WindowSurface::Ptr &surface,
                    const Window::Ptr &window) {
    return std::make_shared<SwapChain>(device, surface, window);
  }

  SwapChain(const Device::Ptr &device, const WindowSurface::Ptr &surface,
            const Window::Ptr &window);

  ~SwapChain();

  SwapChainSupportDetails QuerySwapChainSupport();

  VkSurfaceFormatKHR ChooseSwapSurfaceFormat(
      const std::vector<VkSurfaceFormatKHR> &availableFormats);

  VkPresentModeKHR ChooseSwapPresentMode(
      const std::vector<VkPresentModeKHR> &availablePresentModes);

  VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

  [[nodiscard]] auto GetSwapChainExtent() const { return mSwapChainExtent; }

  [[nodiscard]] auto GetSwapChainImageFormat() const {
    return mSwapChainImageFormat;
  }

  [[nodiscard]] auto GetSwapChainImageCount() const { return mImageCount; }

  [[nodiscard]] auto GetSwapChainImageView(const int index) const {
    return mSwapChainImageViews[index];
  }

  [[nodiscard]] auto GetSwapChain() const { return mSwapChain; }

 private:
  void CreateImageViews();

 private:
  Device::Ptr mDevice{nullptr};
  WindowSurface::Ptr mSurface{nullptr};
  Window::Ptr mWindow{nullptr};
  VkSwapchainKHR mSwapChain{VK_NULL_HANDLE};

  std::vector<VkImage> mSwapChainImages{};
  VkFormat mSwapChainImageFormat{};
  VkExtent2D mSwapChainExtent{};
  uint32_t mImageCount{0};

  std::vector<VkImageView> mSwapChainImageViews{};
};

#endif  // COMMON_SWAP_CHAIN_H_
