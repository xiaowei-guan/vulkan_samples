#ifndef SWAP_CHAIN_H_
#define SWAP_CHAIN_H_

#include "device.h"
#include "window.h"
#include "window_surface.h"



struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;
};

class SwapChain {
public:
  using Ptr = std::shared_ptr<SwapChain>;
  static Ptr create(const Device::Ptr &device,
                    const WindowSurface::Ptr &surface,
                    const Window::Ptr &window) {
    return std::make_shared<SwapChain>(device, surface, window);
  }

  SwapChain(const Device::Ptr &device, const WindowSurface::Ptr &surface,
            const Window::Ptr &window);

  ~SwapChain();

  SwapChainSupportDetails querySwapChainSupport();

  VkSurfaceFormatKHR chooseSwapSurfaceFormat(
      const std::vector<VkSurfaceFormatKHR> &availableFormats);

  VkPresentModeKHR chooseSwapPresentMode(
      const std::vector<VkPresentModeKHR> &availablePresentModes);

  VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

  [[nodiscard]] auto getSwapChainExtent() const { return mSwapChainExtent; }

  [[nodiscard]] auto getSwapChainImageFormat() const {
    return mSwapChainImageFormat;
  }

  [[nodiscard]] auto getSwapChainImageCount() const { return mImageCount; }

  [[nodiscard]] auto getSwapChainImageView(const int index) const {
    return mSwapChainImageViews[index];
  }

  [[nodiscard]] auto getSwapChain() const { return mSwapChain; }

private:
  void createImageViews();

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

#endif
