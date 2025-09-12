
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

#ifndef COMMON_WINDOW_SURFACE_H_
#define COMMON_WINDOW_SURFACE_H_

#include <memory>

#include "common/instance.h"
#include "common/window.h"

class WindowSurface {
 public:
  WindowSurface(const std::shared_ptr<Instance> &instance,
                const std::shared_ptr<Window> &window);
  ~WindowSurface();

  [[nodiscard]] auto GetSurface() const { return vk_surface_; }

 private:
  // To present rendered images to. WIll be backed by the window that opened
  // with GLFW. An entirely optional component in Vulkan, if you just need
  // off-screen rendering.
  VkSurfaceKHR vk_surface_ = VK_NULL_HANDLE;
  std::shared_ptr<Instance> instance_ = nullptr;
};

#endif  // COMMON_WINDOW_SURFACE_H_
