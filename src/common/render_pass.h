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

#ifndef COMMON_RENDER_PASS_H_
#define COMMON_RENDER_PASS_H_

#include <vector>
#include <memory>

#include "common/device.h"

class SubPass {
 public:
  ~SubPass();

  void AddColorAttachmentReference(const VkAttachmentReference &ref);

  void AddInputAttachmentReference(const VkAttachmentReference &ref);

  void SetDepthStencilAttachmentReference(const VkAttachmentReference &ref);

  void BuildSubPassDescription();

  [[nodiscard]] auto GetSubPassDescription() const {
    return mSubPassDescription;
  }

 private:
  VkSubpassDescription mSubPassDescription{};
  std::vector<VkAttachmentReference> mColorAttachmentReferences{};
  std::vector<VkAttachmentReference> mInputAttachmentReferences{};
  VkAttachmentReference mDepthStencilAttachmentReference{};
};

class RenderPass {
 public:
  using Ptr = std::shared_ptr<RenderPass>;

  static Ptr Create(const Device::Ptr &device) {
    return std::make_shared<RenderPass>(device);
  }

  explicit RenderPass(const Device::Ptr &device);

  ~RenderPass();

  void AddSubPass(const SubPass &subpass);

  void AddDependency(const VkSubpassDependency &dependency);

  void AddAttachment(const VkAttachmentDescription &attachmentDes);

  void BuildRenderPass();

  [[nodiscard]] auto GetRenderPass() const { return mRenderPass; }

 private:
  VkRenderPass mRenderPass{VK_NULL_HANDLE};

  std::vector<SubPass> mSubPasses{};
  std::vector<VkSubpassDependency> mDependencies{};
  std::vector<VkAttachmentDescription> mAttachmentDescriptions{};

  Device::Ptr mDevice{nullptr};
};

#endif  // COMMON_RENDER_PASS_H_
