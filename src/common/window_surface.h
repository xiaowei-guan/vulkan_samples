#ifndef WINDOW_SURFACE_H_
#define WINDOW_SURFACE_H_

#include "instance.h"
#include "window.h"
#include <memory>

class WindowSurface {
 public:
  using Ptr = std::shared_ptr<WindowSurface>;
  static Ptr create(const Instance::Ptr &instance, const Window::Ptr &window) {
    return std::make_shared<WindowSurface>(instance, window);
  }

  WindowSurface(const Instance::Ptr &instance, const Window::Ptr &window);
  ~WindowSurface();

  [[nodiscard]] auto getSurface() const { return mSurface; }

 private:
  // To present rendered images to. WIll be backed by the window that opened
  // with GLFW. An entirely optional component in Vulkan, if you just need
  // off-screen rendering.
  VkSurfaceKHR mSurface{VK_NULL_HANDLE};
  Instance::Ptr mInstance{nullptr};
};

#endif
