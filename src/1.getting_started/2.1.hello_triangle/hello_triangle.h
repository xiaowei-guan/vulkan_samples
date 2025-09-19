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

  void Run();

 private:
  void InitWindow();

  void InitVulkan();

  void MainLoop();

  void DrawFrame();

  void CleanUp();

 private:
  void CreateRenderPass();
  void CreatePipeline();

 private:
  std::shared_ptr<Window> window_ = nullptr;
  std::shared_ptr<Instance> instance_ = nullptr;
  std::shared_ptr<WindowSurface> window_surface_ = nullptr;
  std::shared_ptr<Device> device_ = nullptr;
  std::shared_ptr<SwapChain> swap_chain_ = nullptr;
  std::shared_ptr<RenderPass> render_pass_ = nullptr;
  std::shared_ptr<GraphicsPipeline> graphics_pipeline_ = nullptr;
  std::shared_ptr<FrameBuffers> frame_buffers_ = nullptr;
  std::shared_ptr<CommandPool> command_pool_ = nullptr;

  std::vector<std::shared_ptr<CommandBuffer>> mCommandBuffers{};

  uint32_t current_frame_ = 0;
  std::vector<std::shared_ptr<Semaphore>> image_available_semaphores_;
  std::vector<std::shared_ptr<Semaphore>> render_finished_semaphores_;
  std::vector<std::shared_ptr<Fence>> fences_;
};

#endif
