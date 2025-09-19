#include "hello_triangle.h"

void HelloTriangleApplication::Run() {
  InitWindow();
  InitVulkan();
  MainLoop();
  CleanUp();
}

void HelloTriangleApplication::InitWindow() {
  window_ = std::make_unique<Window>(WIDTH, HEIGHT);
}

void HelloTriangleApplication::InitVulkan() {
  instance_ = std::make_shared<Instance>(true);
  // The window surface can actually influence the physical device selection.
  // So we create it after the instance creation.
  window_surface_ = std::make_shared<WindowSurface>(instance_, window_);
  device_ = std::make_shared<Device>(instance_, window_surface_);
  swap_chain_ = std::make_shared<SwapChain>(device_, window_surface_, window_);

  render_pass_ = std::make_shared<RenderPass>(device_);
  CreateRenderPass();
  graphics_pipeline_ =
      std::make_shared<GraphicsPipeline>(device_, render_pass_);
  CreatePipeline();

  frame_buffers_ =
      std::make_shared<FrameBuffers>(device_, swap_chain_, render_pass_);
  command_pool_ = std::make_shared<CommandPool>(device_);

  mCommandBuffers.resize(swap_chain_->GetSwapChainImageCount());
  for (int i = 0; i < swap_chain_->GetSwapChainImageCount(); ++i) {
    mCommandBuffers[i] =
        std::make_shared<CommandBuffer>(device_, command_pool_);

    mCommandBuffers[i]->Begin();

    VkRenderPassBeginInfo renderBeginInfo{};
    renderBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderBeginInfo.renderPass = render_pass_->GetRenderPass();
    renderBeginInfo.framebuffer = frame_buffers_->GetFrameBuffer(i);
    renderBeginInfo.renderArea.offset = {0, 0};
    renderBeginInfo.renderArea.extent = swap_chain_->GetSwapChainExtent();

    VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
    renderBeginInfo.clearValueCount = 1;
    renderBeginInfo.pClearValues = &clearColor;

    mCommandBuffers[i]->BeginRenderPass(renderBeginInfo);

    mCommandBuffers[i]->BindGraphicPipeline(graphics_pipeline_->GetPipeline());

    mCommandBuffers[i]->Draw(3);

    mCommandBuffers[i]->EndRenderPass();

    mCommandBuffers[i]->End();
  }

  for (int i = 0; i < swap_chain_->GetSwapChainImageCount(); ++i) {
    auto imageSemaphore = std::make_shared<Semaphore>(device_);
    image_available_semaphores_.push_back(imageSemaphore);

    auto renderSemaphore = std::make_shared<Semaphore>(device_);
    render_finished_semaphores_.push_back(renderSemaphore);

    auto fence = std::make_shared<Fence>(device_);
    fences_.push_back(fence);
  }
}

void HelloTriangleApplication::CreateRenderPass() {
  // 1.Attachment description.
  VkAttachmentDescription colorAttachment{};
  colorAttachment.format = swap_chain_->GetSwapChainImageFormat();
  colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  render_pass_->AddAttachment(colorAttachment);

  // 2.Subpasses and attachment references.
  VkAttachmentReference colorAttachmentRef{};
  colorAttachmentRef.attachment = 0;
  colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  SubPass subpass{};
  subpass.AddColorAttachmentReference(colorAttachmentRef);
  subpass.BuildSubPassDescription();

  render_pass_->AddSubPass(subpass);

  // 3.Subpass dependencies.
  VkSubpassDependency dependency{};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask = 0;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
                             VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  render_pass_->AddDependency(dependency);

  // 4.Render pass
  render_pass_->BuildRenderPass();
}

void HelloTriangleApplication::CreatePipeline() {
  // 1.Shader stages: the shader modules that define the functionality of the
  // programmable stages of the graphics pipeline.
  std::vector<VkPipelineShaderStageCreateInfo> shaderStages{};

  auto vertShaderStageInfo = std::make_shared<ShaderStageInfo>(
      device_, "./data/2.1.hello_triangle/shader.vert.spv",
      VK_SHADER_STAGE_VERTEX_BIT, "main");
  shaderStages.push_back(vertShaderStageInfo->GetShaderStageInfo());

  auto fragShaderStageInfo = std::make_shared<ShaderStageInfo>(
      device_, "./data/2.1.hello_triangle/shader.frag.spv",
      VK_SHADER_STAGE_FRAGMENT_BIT, "main");
  shaderStages.push_back(fragShaderStageInfo->GetShaderStageInfo());

  graphics_pipeline_->SetShaderStages(shaderStages);

  // 2.Fixed-function state: all of the structures that define the
  // fixed-function stages of the pipeline
  //
  // 2.1.Dynamic states.
  // std::vector<VkDynamicState> dynamicStates = {
  //     VK_DYNAMIC_STATE_VIEWPORT,
  //     VK_DYNAMIC_STATE_SCISSOR
  // };

  // VkPipelineDynamicStateCreateInfo dynamicState{};
  // dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  // dynamicState.dynamicStateCount =
  // static_cast<uint32_t>(dynamicStates.size()); dynamicState.pDynamicStates =
  // dynamicStates.data();

  // 2.2.Vertex input.
  // Describes the format of the vertex data that will be passed to the vertex
  // shader.
  graphics_pipeline_->vertex_input_info.vertexBindingDescriptionCount = 0;
  graphics_pipeline_->vertex_input_info.pVertexBindingDescriptions = nullptr;
  graphics_pipeline_->vertex_input_info.vertexAttributeDescriptionCount = 0;
  graphics_pipeline_->vertex_input_info.pVertexAttributeDescriptions = nullptr;

  // 2.3.Input assembly.
  // Describes two things: what kind of geometry will be drawn from the vertices
  // and if primitive restart should be enabled.
  graphics_pipeline_->input_assembly.topology =
      VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  graphics_pipeline_->input_assembly.primitiveRestartEnable = VK_FALSE;

  // 2.4.Viewports and scissors.
  // A viewport basically describes the region of the framebuffer that the
  // output will be rendered to.
  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = (float)swap_chain_->GetSwapChainExtent().width;
  viewport.height = (float)swap_chain_->GetSwapChainExtent().height;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  // Scissor rectangles define in which regions pixels will actually be stored.
  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = swap_chain_->GetSwapChainExtent();

  // Set the viewport and scissor in pipeline.
  graphics_pipeline_->SetViewports({viewport});
  graphics_pipeline_->SetScissors({scissor});

  // 2.5.Rasterizer.
  // The rasterizer takes the geometry that is shaped by the vertices from the
  // vertex shader and turns it into fragments to be colored by the fragment
  // shader.
  graphics_pipeline_->rasterizer.depthClampEnable = VK_FALSE;
  graphics_pipeline_->rasterizer.rasterizerDiscardEnable = VK_FALSE;

  graphics_pipeline_->rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
  graphics_pipeline_->rasterizer.lineWidth = 1.0f;
  graphics_pipeline_->rasterizer.cullMode = VK_CULL_MODE_NONE;
  graphics_pipeline_->rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;

  graphics_pipeline_->rasterizer.depthBiasEnable = VK_FALSE;
  graphics_pipeline_->rasterizer.depthBiasConstantFactor = 0.0f;
  graphics_pipeline_->rasterizer.depthBiasClamp = 0.0f;
  graphics_pipeline_->rasterizer.depthBiasSlopeFactor = 0.0f;

  // 2.6.Multi-sampling.
  // Multi-sampling is an anti-aliasing method used to make edges look less
  // jagged.
  graphics_pipeline_->multisampling.sampleShadingEnable = VK_FALSE;
  graphics_pipeline_->multisampling.rasterizationSamples =
      VK_SAMPLE_COUNT_1_BIT;
  graphics_pipeline_->multisampling.minSampleShading = 1.0f;
  graphics_pipeline_->multisampling.pSampleMask = nullptr;
  graphics_pipeline_->multisampling.alphaToCoverageEnable = VK_FALSE;
  graphics_pipeline_->multisampling.alphaToOneEnable = VK_FALSE;

  // 2.7. TODO: Depth and stencil testing.
  // VkPipelineDepthStencilStateCreateInfo depthStencil{};

  // 2.8.Color blending.
  // After a fragment shader has returned a color, it needs to be combined with
  // the color that is already in the framebuffer. 2.8.1
  // 'VkPipelineColorBlendAttachmentState' contains the configuration per
  // attached framebuffer Using the following parameteres, we can get:
  //
  // if (blendEnable) {
  //     finalColor.rgb = (srcColorBlendFactor * newColor.rgb) <colorBlendOp>
  //     (dstColorBlendFactor * oldColor.rgb); finalColor.a =
  //     (srcAlphaBlendFactor * newColor.a) <alphaBlendOp> (dstAlphaBlendFactor
  //     * oldColor.a);
  // } else {
  //     finalColor = newColor;
  // }
  VkPipelineColorBlendAttachmentState colorBlendAttachment{};
  colorBlendAttachment.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  colorBlendAttachment.blendEnable = VK_FALSE;
  colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
  colorBlendAttachment.dstColorBlendFactor =
      VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
  colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
  colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

  graphics_pipeline_->AddBlendAttachment(colorBlendAttachment);

  // 2.8.2 'VkPipelineColorBlendStateCreateInfo' contains the global color
  // blending settings
  //
  // if logicIpEnable == VK_TRUE, the 'VkPipelineColorBlendAttachmentState' will
  // be banned, but 'colorWriteMask' is valid.
  graphics_pipeline_->color_blending.logicOpEnable =
      VK_FALSE;  // If usr the bitwise combination method of blending.
  graphics_pipeline_->color_blending.logicOp =
      VK_LOGIC_OP_COPY;  // If specify the bitwise operation.
  graphics_pipeline_->color_blending.blendConstants[0] = 0.0f;
  graphics_pipeline_->color_blending.blendConstants[1] = 0.0f;
  graphics_pipeline_->color_blending.blendConstants[2] = 0.0f;
  graphics_pipeline_->color_blending.blendConstants[3] = 0.0f;

  // 3.Pipeline layout: the uniform and push values referenced by the shader
  // that can be updated at draw time
  graphics_pipeline_->pipeline_layout_info.setLayoutCount = 0;     // Optional
  graphics_pipeline_->pipeline_layout_info.pSetLayouts = nullptr;  // Optional
  graphics_pipeline_->pipeline_layout_info.pushConstantRangeCount =
      0;  // Optional
  graphics_pipeline_->pipeline_layout_info.pPushConstantRanges =
      nullptr;  // Optional

  // 4.Create pipeline
  graphics_pipeline_->BuildPipeline();
}

void HelloTriangleApplication::MainLoop() {
  while (!window_->WindowShouldClose()) {
    window_->PollEvents();
    DrawFrame();
  }

  vkDeviceWaitIdle(device_->GetDevice());
}

void HelloTriangleApplication::DrawFrame() {
  fences_[current_frame_]->Block();

  // Acquiring an image from the swap chain.
  uint32_t imageIndex{0};
  vkAcquireNextImageKHR(
      device_->GetDevice(), swap_chain_->GetSwapChain(), UINT64_MAX,
      image_available_semaphores_[current_frame_]->GetSemaphore(),
      VK_NULL_HANDLE, &imageIndex);

  // Submitting the command buffer.
  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkSemaphore waitSemaphores[] = {
      image_available_semaphores_[current_frame_]->GetSemaphore()};
  VkPipelineStageFlags waitStages[] = {
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSemaphores;
  submitInfo.pWaitDstStageMask = waitStages;

  auto commandBuffer = mCommandBuffers[imageIndex]->GetCommandBuffer();
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffer;

  VkSemaphore signalSemaphores[] = {
      render_finished_semaphores_[current_frame_]->GetSemaphore()};
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = signalSemaphores;

  fences_[current_frame_]->ResetFence();
  if (vkQueueSubmit(device_->GetGraphicsQueue(), 1, &submitInfo,
                    fences_[current_frame_]->GetFence()) != VK_SUCCESS) {
    throw std::runtime_error("failed to submit draw command buffer!");
  }

  // Presentation
  VkPresentInfoKHR presentInfo{};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = signalSemaphores;

  VkSwapchainKHR swapChains[] = {swap_chain_->GetSwapChain()};
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapChains;

  presentInfo.pImageIndices = &imageIndex;

  vkQueuePresentKHR(device_->GetPresentQueue(), &presentInfo);

  // update current frame index
  current_frame_ = (current_frame_ + 1) % swap_chain_->GetSwapChainImageCount();
}

void HelloTriangleApplication::CleanUp() {
  graphics_pipeline_.reset();
  swap_chain_.reset();
  device_.reset();
  window_surface_.reset();
  instance_.reset();
  window_.reset();
}
