#ifndef DEVICE_H_
#define DEVICE_H_

#include "instance.h"
#include "window_surface.h"
#include <optional>

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
  static Ptr create(const Instance::Ptr &instance,
                    const WindowSurface::Ptr &windowSurface) {
    return std::make_shared<Device>(instance, windowSurface);
  }

  Device(const Instance::Ptr &instance,
         const WindowSurface::Ptr &windowSurface);

  ~Device();

  void pickPhysicalDevice();

  bool isDeviceSuitable(VkPhysicalDevice device);

  int rateDeviceSuitability(VkPhysicalDevice device);

  void findQueueFamilies(VkPhysicalDevice device);

  void createLogicalDevice();

  bool checkDeviceExtensionSupport(VkPhysicalDevice device);

  [[nodiscard]] auto getDevice() const { return mDevice; }
  [[nodiscard]] auto getPhysicalDevice() const { return mPhysicalDevice; }
  [[nodiscard]] auto getQueueFamilyIndices() const {
    return mQueueFamilyIndices;
  }
  [[nodiscard]] auto getGraphicsQueue() const { return mGraphicsQueue; }
  [[nodiscard]] auto getPresentQueue() const { return mPresentQueue; }

 private:
  VkPhysicalDevice mPhysicalDevice{VK_NULL_HANDLE};
  Instance::Ptr mInstance{nullptr};
  WindowSurface::Ptr mWindowSurface{nullptr};
  VkDevice mDevice{VK_NULL_HANDLE};  // logical device handle
  QueueFamilyIndices mQueueFamilyIndices{};
  VkQueue mGraphicsQueue{VK_NULL_HANDLE};
  VkQueue mPresentQueue{VK_NULL_HANDLE};
};

#endif