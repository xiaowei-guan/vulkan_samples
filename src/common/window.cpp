#include "window.h"

#include <iostream>

Window::Window(const uint32_t &width, const uint32_t &height) {
  mWidth = width;
  mHeight = height;

  // Initializes the GLFW library.
  glfwInit();

  // Don't initialize OpenGL context. Disable resizing windows.
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  mWindow =
      glfwCreateWindow(mWidth, mHeight, "vulkan window", nullptr, nullptr);
  if (!mWindow) {
    std::cerr << "Error: failed to create window" << std::endl;
  }
}

Window::~Window() {
  glfwDestroyWindow(mWindow);
  glfwTerminate();
}

bool Window::windowShouldClose() { return glfwWindowShouldClose(mWindow); }

void Window::pollEvents() { return glfwPollEvents(); }
