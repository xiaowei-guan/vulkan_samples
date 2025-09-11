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

#ifndef COMMON_INSTANCE_H_
#define COMMON_INSTANCE_H_

#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

class Instance {
 public:
  using Ptr = std::shared_ptr<Instance>;
  static Ptr create(bool enableValidationLayer) {
    return std::make_shared<Instance>(enableValidationLayer);
  }

  explicit Instance(bool enableValidationLayer);

  ~Instance();

  void CheckExtensionSupport();
  std::vector<const char *> GetRequiredExtensions();

  bool CheckValidationLayerSupport();
  void SetupDebugger();

  [[nodiscard]] VkInstance GetInstance() const { return mInstance; }
  [[nodiscard]] bool IsValidationLayerEnabled() const {
    return mEnableValidationLayer;
  }

 private:
  VkInstance mInstance{VK_NULL_HANDLE};
  bool mEnableValidationLayer{false};
  VkDebugUtilsMessengerEXT mDebugger{VK_NULL_HANDLE};
};

#endif  // COMMON_INSTANCE_H_

