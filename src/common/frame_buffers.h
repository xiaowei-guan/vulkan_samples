#ifndef FRAME_BUFFERS_H_
#define FRAME_BUFFERS_H_

#include "device.h"
#include "render_pass.h"
#include "swap_chain.h"

class FrameBuffers {
 public:
  using Ptr = std::shared_ptr<FrameBuffers>;
  static Ptr create(const Device::Ptr &device, const SwapChain::Ptr &swapChain,
                    const RenderPass::Ptr &renderPass) {
    return std::make_shared<FrameBuffers>(device, swapChain, renderPass);
  }

  FrameBuffers(const Device::Ptr &device, const SwapChain::Ptr &swapChain,
               const RenderPass::Ptr &renderPass);
  ~FrameBuffers();

  [[nodiscard]] auto getFrameBuffer(const size_t index) const {
    return mSwapChainFramebuffers[index];
  }

 private:
  std::vector<VkFramebuffer> mSwapChainFramebuffers;

  Device::Ptr mDevice{nullptr};
  SwapChain::Ptr mSwapChain{nullptr};
  RenderPass::Ptr mRenderPass{nullptr};
};

#endif
