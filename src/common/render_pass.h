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

#include <memory>
#include <vector>

#include "common/device.h"

class SubPass {
 public:
  ~SubPass();

  void AddColorAttachmentReference(const VkAttachmentReference &ref);

  void AddInputAttachmentReference(const VkAttachmentReference &ref);

  void SetDepthStencilAttachmentReference(const VkAttachmentReference &ref);

  void BuildSubPassDescription();

  [[nodiscard]] auto GetSubPassDescription() const {
    return vk_subpass_description_;
  }

 private:
  VkSubpassDescription vk_subpass_description_;
  std::vector<VkAttachmentReference> color_attachment_references_;
  std::vector<VkAttachmentReference> input_attachment_references_;
  VkAttachmentReference depth_stencil_attachment_reference_;
};

class RenderPass {
 public:
  explicit RenderPass(const std::shared_ptr<Device> &device);

  ~RenderPass();

  void AddSubPass(const SubPass &subpass);

  void AddDependency(const VkSubpassDependency &dependency);

  void AddAttachment(const VkAttachmentDescription &attachmentDes);

  void BuildRenderPass();

  [[nodiscard]] auto GetRenderPass() const { return vk_render_pass_; }

 private:
  VkRenderPass vk_render_pass_ = VK_NULL_HANDLE;
  std::vector<SubPass> subpass_;
  std::vector<VkSubpassDependency> vk_subpass_dependencies_;
  std::vector<VkAttachmentDescription> vk_attachment_descriptions_;
  std::shared_ptr<Device> device_ = nullptr;
};

#endif  // COMMON_RENDER_PASS_H_
