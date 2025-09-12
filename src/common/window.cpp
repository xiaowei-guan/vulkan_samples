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

#include "common/window.h"

#include <iostream>

Window::Window(const uint32_t &width, const uint32_t &height)
    : width_(width), height_(height) {
  // Initializes the GLFW library.
  glfwInit();

  // Don't initialize OpenGL context. Disable resizing windows.
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  glfw_window_ =
      glfwCreateWindow(width_, height_, "vulkan window", nullptr, nullptr);
  if (!glfw_window_) {
    std::cerr << "Error: failed to create window" << std::endl;
  }
}

Window::~Window() {
  glfwDestroyWindow(glfw_window_);
  glfwTerminate();
}

bool Window::WindowShouldClose() { return glfwWindowShouldClose(glfw_window_); }

void Window::PollEvents() { return glfwPollEvents(); }
