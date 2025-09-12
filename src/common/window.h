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

#ifndef COMMON_WINDOW_H_
#define COMMON_WINDOW_H_

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <memory>

class Window {
 public:
  Window(const uint32_t &width, const uint32_t &height);

  ~Window();

  bool WindowShouldClose();

  void PollEvents();

  [[nodiscard]] auto GetWindow() const { return glfw_window_; }

 private:
  uint32_t width_ = 0;
  uint32_t height_ = 0;
  GLFWwindow *glfw_window_ = nullptr;
};

#endif  // COMMON_WINDOW_H_
