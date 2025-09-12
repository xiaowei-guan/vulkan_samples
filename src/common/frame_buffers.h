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

#ifndef COMMON_FRAME_BUFFERS_H_
#define COMMON_FRAME_BUFFERS_H_

#include <memory>
#include <vector>

#include "common/device.h"
#include "common/render_pass.h"
#include "common/swap_chain.h"

class FrameBuffers {
 public:
  FrameBuffers(const std::shared_ptr<Device> &device,
               const std::shared_ptr<SwapChain> &swapChain,
               const std::shared_ptr<RenderPass> &renderPass);
  ~FrameBuffers();

  [[nodiscard]] auto GetFrameBuffer(const size_t index) const {
    return swap_chain_frame_buffers_[index];
  }

 private:
  std::vector<VkFramebuffer> swap_chain_frame_buffers_;
  std::shared_ptr<Device> device_ = nullptr;
  std::shared_ptr<SwapChain> swap_chain_ = nullptr;
  std::shared_ptr<RenderPass> render_pass_ = nullptr;
};

#endif  // COMMON_FRAME_BUFFERS_H_
