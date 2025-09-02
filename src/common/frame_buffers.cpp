#include "frame_buffers.h"

FrameBuffers::FrameBuffers(const Device::Ptr &device,
                           const SwapChain::Ptr &swapChain,
                           const RenderPass::Ptr &renderPass) {
  mDevice = device;
  mSwapChain = swapChain;
  mRenderPass = renderPass;

  size_t imageCount = mSwapChain->getSwapChainImageCount();
  mSwapChainFramebuffers.resize(imageCount);

  for (size_t i = 0; i < imageCount; ++i) {
    VkImageView attachments[] = {mSwapChain->getSwapChainImageView(i)};

    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = mRenderPass->getRenderPass();
    framebufferInfo.attachmentCount = 1;
    framebufferInfo.pAttachments = attachments;
    framebufferInfo.width = mSwapChain->getSwapChainExtent().width;
    framebufferInfo.height = mSwapChain->getSwapChainExtent().height;
    framebufferInfo.layers = 1;

    if (vkCreateFramebuffer(mDevice->getDevice(), &framebufferInfo, nullptr,
                            &mSwapChainFramebuffers[i]) != VK_SUCCESS) {
      throw std::runtime_error("Error: failed to create framebuffer!");
    }
  }
}

FrameBuffers::~FrameBuffers() {
  for (auto framebuffer : mSwapChainFramebuffers) {
    vkDestroyFramebuffer(mDevice->getDevice(), framebuffer, nullptr);
  }

  mDevice.reset();
  mSwapChain.reset();
  mRenderPass.reset();
}
