#ifndef HELLO_TRIANGLE_H_
#define HELLO_TRIANGLE_H_

#include "common/command_buffer.h"
#include "common/device.h"
#include "common/frame_buffers.h"
#include "common/graphics_pipeline.h"
#include "common/instance.h"
#include "common/render_pass.h"
#include "common/swap_chain.h"
#include "common/sync_objects.h"
#include "common/window.h"
#include "common/window_surface.h"

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

class HelloTriangleApplication {
 public:
  HelloTriangleApplication() = default;

  ~HelloTriangleApplication() = default;

  void run();

 private:
  void initWindow();

  void initVulkan();

  void mainLoop();

  void drawFrame();

  void cleanUp();

 private:
  void createRenderPass();
  void createPipeline();

 private:
  Window::Ptr mWindow{nullptr};
  Instance::Ptr mInstance{nullptr};
  WindowSurface::Ptr mSurface{nullptr};
  Device::Ptr mDevice{nullptr};
  SwapChain::Ptr mSwapChain{nullptr};
  RenderPass::Ptr mRenderPass{nullptr};
  GraphicsPipeline::Ptr mGraphicsPipeline{nullptr};
  FrameBuffers::Ptr mFrameBuffers{nullptr};
  CommandPool::Ptr mCommandPool{nullptr};

  std::vector<CommandBuffer::Ptr> mCommandBuffers{};

  uint32_t mCurrentFrame{0};
  std::vector<Semaphore::Ptr> mImageAvailableSemaphores{};
  std::vector<Semaphore::Ptr> mRenderFinishedSemaphores{};
  std::vector<Fence::Ptr> mFences{};
};

#endif
