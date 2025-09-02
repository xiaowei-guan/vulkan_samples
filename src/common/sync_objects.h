#ifndef SYNC_OBJECTS_H_
#define SYNC_OBJECTS_H_

#include "device.h"



class Semaphore {
public:
  using Ptr = std::shared_ptr<Semaphore>;
  static Ptr create(const Device::Ptr &device) {
    return std::make_shared<Semaphore>(device);
  }

  Semaphore(const Device::Ptr &device);

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

  Fence(const Device::Ptr &device, bool signaled = true);

  ~Fence();

  void resetFence();

  void block(uint64_t timeout = UINT64_MAX);

  [[nodiscard]] auto getFence() const { return mFence; }

private:
  VkFence mFence{VK_NULL_HANDLE};
  Device::Ptr mDevice{nullptr};
};

#endif
