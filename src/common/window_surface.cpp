#include "window_surface.h"

#include <stdexcept>

WindowSurface::WindowSurface(const Instance::Ptr &instance,
                             const Window::Ptr &window) {
  mInstance = instance;
  if (glfwCreateWindowSurface(mInstance->getInstance(), window->getWindow(),
                              nullptr, &mSurface) != VK_SUCCESS) {
    throw std::runtime_error("Error: Failed to create window surface!");
  }
}

WindowSurface::~WindowSurface() {
  vkDestroySurfaceKHR(mInstance->getInstance(), mSurface, nullptr);
  mInstance.reset();
}
