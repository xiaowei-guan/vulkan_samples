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

#include "common/swap_chain.h"
#include <stdexcept>
#include <limits>
#include <vector>

SwapChain::SwapChain(const Device::Ptr &device,
                     const WindowSurface::Ptr &surface,
                     const Window::Ptr &window) {
  mDevice = device;
  mSurface = surface;
  mWindow = window;

  SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport();

  VkSurfaceFormatKHR surfaceFormat =
      ChooseSwapSurfaceFormat(swapChainSupport.formats);
  VkPresentModeKHR presentMode =
      ChooseSwapPresentMode(swapChainSupport.presentModes);
  VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);

  mImageCount = swapChainSupport.capabilities.minImageCount + 1;
  if (swapChainSupport.capabilities.maxImageCount > 0 &&
      mImageCount > swapChainSupport.capabilities.maxImageCount) {
    mImageCount = swapChainSupport.capabilities.maxImageCount;
  }

  VkSwapchainCreateInfoKHR createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  createInfo.surface = mSurface->GetSurface();
  createInfo.minImageCount = mImageCount;
  createInfo.imageFormat = surfaceFormat.format;
  createInfo.imageColorSpace = surfaceFormat.colorSpace;
  createInfo.imageExtent = extent;
  // This is always 1 unless you are developing a stereoscopic 3D application.
  createInfo.imageArrayLayers = 1;
  createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  // Specify how to handle swap chain images that will be used across multiple
  // queue families.
  std::vector<uint32_t> queueFamilyIndices = {
      mDevice->GetQueueFamilyIndices().graphicsFamily.value(),
      mDevice->GetQueueFamilyIndices().presentFamily.value()};
  if (queueFamilyIndices[0] != queueFamilyIndices[1]) {
    createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    createInfo.queueFamilyIndexCount = 2;
    createInfo.pQueueFamilyIndices = queueFamilyIndices.data();
  } else {
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0;      // Optional
    createInfo.pQueueFamilyIndices = nullptr;  // Optional
  }

  createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
  createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  createInfo.presentMode = presentMode;
  createInfo.clipped = VK_TRUE;
  createInfo.oldSwapchain = VK_NULL_HANDLE;

  if (vkCreateSwapchainKHR(mDevice->GetDevice(), &createInfo, nullptr,
                           &mSwapChain) != VK_SUCCESS) {
    throw std::runtime_error("failed to create swap chain!");
  }

  // Retrieving the swap chain images.
  vkGetSwapchainImagesKHR(mDevice->GetDevice(), mSwapChain, &mImageCount,
                          nullptr);
  mSwapChainImages.resize(mImageCount);
  vkGetSwapchainImagesKHR(mDevice->GetDevice(), mSwapChain, &mImageCount,
                          mSwapChainImages.data());

  mSwapChainImageFormat = surfaceFormat.format;
  mSwapChainExtent = extent;

  CreateImageViews();
}

SwapChain::~SwapChain() {
  for (auto &imageView : mSwapChainImageViews) {
    vkDestroyImageView(mDevice->GetDevice(), imageView, nullptr);
  }
  if (mSwapChain) {
    vkDestroySwapchainKHR(mDevice->GetDevice(), mSwapChain, nullptr);
  }
  mDevice.reset();
  mSurface.reset();
  mWindow.reset();
}

SwapChainSupportDetails SwapChain::QuerySwapChainSupport() {
  SwapChainSupportDetails details;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mDevice->GetPhysicalDevice(),
                                            mSurface->GetSurface(),
                                            &details.capabilities);

  uint32_t formatCount;
  vkGetPhysicalDeviceSurfaceFormatsKHR(mDevice->GetPhysicalDevice(),
                                       mSurface->GetSurface(), &formatCount,
                                       nullptr);
  if (formatCount != 0) {
    details.formats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(mDevice->GetPhysicalDevice(),
                                         mSurface->GetSurface(), &formatCount,
                                         details.formats.data());
  }

  uint32_t presentModeCount;
  vkGetPhysicalDeviceSurfacePresentModesKHR(mDevice->GetPhysicalDevice(),
                                            mSurface->GetSurface(),
                                            &presentModeCount, nullptr);
  if (presentModeCount != 0) {
    details.presentModes.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        mDevice->GetPhysicalDevice(), mSurface->GetSurface(), &presentModeCount,
        details.presentModes.data());
  }

  return details;
}

VkSurfaceFormatKHR SwapChain::ChooseSwapSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR> &availableFormats) {
  for (const auto &availableFormat : availableFormats) {
    if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
        availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      return availableFormat;
    }
  }

  return availableFormats[0];
}

VkPresentModeKHR SwapChain::ChooseSwapPresentMode(
    const std::vector<VkPresentModeKHR> &availablePresentModes) {
  for (const auto &availablePresentMode : availablePresentModes) {
    if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
      return availablePresentMode;
    }
  }

  return VK_PRESENT_MODE_FIFO_KHR;
}

// Vulkan requires that the swap chain extent specified in pixels.
VkExtent2D SwapChain::ChooseSwapExtent(
    const VkSurfaceCapabilitiesKHR &capabilities) {
  if (capabilities.currentExtent.width !=
      std::numeric_limits<uint32_t>::max()) {
    return capabilities.currentExtent;
  } else {
    int width = 0, height = 0;
    glfwGetFramebufferSize(mWindow->GetWindow(), &width, &height);

    VkExtent2D actualExtent = {static_cast<uint32_t>(width),
                               static_cast<uint32_t>(height)};

    actualExtent.width =
        std::clamp(actualExtent.width, capabilities.minImageExtent.width,
                   capabilities.maxImageExtent.width);
    actualExtent.height =
        std::clamp(actualExtent.height, capabilities.minImageExtent.height,
                   capabilities.maxImageExtent.height);

    return actualExtent;
  }
}

void SwapChain::CreateImageViews() {
  mSwapChainImageViews.resize(mImageCount);

  for (size_t i = 0; i < mImageCount; i++) {
    VkImageViewCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = mSwapChainImages[i];
    // The viewType parameter allows you to treat images as 1D textures, 2D
    // textures, 3D textures and cube maps.
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = mSwapChainImageFormat;

    createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(mDevice->GetDevice(), &createInfo, nullptr,
                          &mSwapChainImageViews[i]) != VK_SUCCESS) {
      throw std::runtime_error("failed to create image views!");
    }
  }
}
