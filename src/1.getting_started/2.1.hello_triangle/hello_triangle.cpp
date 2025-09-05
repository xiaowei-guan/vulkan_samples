#include "hello_triangle.h"

void HelloTriangleApplication::Run() {
  InitWindow();
  InitVulkan();
  MainLoop();
  CleanUp();
}

void HelloTriangleApplication::InitWindow() {
  mWindow = Window::create(WIDTH, HEIGHT);
}

void HelloTriangleApplication::InitVulkan() {
  mInstance = Instance::create(true);
  // The window surface can actually influence the physical device selection.
  // So we create it after the instance creation.
  mSurface = WindowSurface::create(mInstance, mWindow);
  mDevice = Device::create(mInstance, mSurface);
  mSwapChain = SwapChain::create(mDevice, mSurface, mWindow);

  mRenderPass = RenderPass::create(mDevice);
  CreateRenderPass();
  mGraphicsPipeline = GraphicsPipeline::create(mDevice, mRenderPass);
  CreatePipeline();

  mFrameBuffers = FrameBuffers::create(mDevice, mSwapChain, mRenderPass);
  mCommandPool = CommandPool::create(mDevice);

  mCommandBuffers.resize(mSwapChain->getSwapChainImageCount());
  for (int i = 0; i < mSwapChain->getSwapChainImageCount(); ++i) {
    mCommandBuffers[i] = CommandBuffer::create(mDevice, mCommandPool);

    mCommandBuffers[i]->begin();

    VkRenderPassBeginInfo renderBeginInfo{};
    renderBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderBeginInfo.renderPass = mRenderPass->getRenderPass();
    renderBeginInfo.framebuffer = mFrameBuffers->getFrameBuffer(i);
    renderBeginInfo.renderArea.offset = {0, 0};
    renderBeginInfo.renderArea.extent = mSwapChain->getSwapChainExtent();

    VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
    renderBeginInfo.clearValueCount = 1;
    renderBeginInfo.pClearValues = &clearColor;

    mCommandBuffers[i]->beginRenderPass(renderBeginInfo);

    mCommandBuffers[i]->bindGraphicPipeline(mGraphicsPipeline->getPipeline());

    mCommandBuffers[i]->draw(3);

    mCommandBuffers[i]->endRenderPass();

    mCommandBuffers[i]->end();
  }

  for (int i = 0; i < mSwapChain->getSwapChainImageCount(); ++i) {
    auto imageSemaphore = Semaphore::create(mDevice);
    mImageAvailableSemaphores.push_back(imageSemaphore);

    auto renderSemaphore = Semaphore::create(mDevice);
    mRenderFinishedSemaphores.push_back(renderSemaphore);

    auto fence = Fence::create(mDevice);
    mFences.push_back(fence);
  }
}

void HelloTriangleApplication::CreateRenderPass() {
  // 1.Attachment description.
  VkAttachmentDescription colorAttachment{};
  colorAttachment.format = mSwapChain->getSwapChainImageFormat();
  colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  mRenderPass->addAttachment(colorAttachment);

  // 2.Subpasses and attachment references.
  VkAttachmentReference colorAttachmentRef{};
  colorAttachmentRef.attachment = 0;
  colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  SubPass subpass{};
  subpass.addColorAttachmentReference(colorAttachmentRef);
  subpass.buildSubPassDescription();

  mRenderPass->addSubPass(subpass);

  // 3.Subpass dependencies.
  VkSubpassDependency dependency{};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask = 0;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
                             VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  mRenderPass->addDependency(dependency);

  // 4.Render pass
  mRenderPass->buildRenderPass();
}

void HelloTriangleApplication::CreatePipeline() {
  // 1.Shader stages: the shader modules that define the functionality of the
  // programmable stages of the graphics pipeline.
  std::vector<VkPipelineShaderStageCreateInfo> shaderStages{};

  auto vertShaderStageInfo = ShaderStageInfo::create(
      mDevice, "./data/2.1.hello_triangle/shader.vert.spv", VK_SHADER_STAGE_VERTEX_BIT, "main");
  shaderStages.push_back(vertShaderStageInfo->getShaderStageInfo());

  auto fragShaderStageInfo = ShaderStageInfo::create(
      mDevice, "./data/2.1.hello_triangle/shader.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT, "main");
  shaderStages.push_back(fragShaderStageInfo->getShaderStageInfo());

  mGraphicsPipeline->setShaderStages(shaderStages);

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
  mGraphicsPipeline->mVertexInputInfo.vertexBindingDescriptionCount = 0;
  mGraphicsPipeline->mVertexInputInfo.pVertexBindingDescriptions = nullptr;
  mGraphicsPipeline->mVertexInputInfo.vertexAttributeDescriptionCount = 0;
  mGraphicsPipeline->mVertexInputInfo.pVertexAttributeDescriptions = nullptr;

  // 2.3.Input assembly.
  // Describes two things: what kind of geometry will be drawn from the vertices
  // and if primitive restart should be enabled.
  mGraphicsPipeline->mInputAssembly.topology =
      VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  mGraphicsPipeline->mInputAssembly.primitiveRestartEnable = VK_FALSE;

  // 2.4.Viewports and scissors.
  // A viewport basically describes the region of the framebuffer that the
  // output will be rendered to.
  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = (float)mSwapChain->getSwapChainExtent().width;
  viewport.height = (float)mSwapChain->getSwapChainExtent().height;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  // Scissor rectangles define in which regions pixels will actually be stored.
  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = mSwapChain->getSwapChainExtent();

  // Set the viewport and scissor in pipeline.
  mGraphicsPipeline->setViewports({viewport});
  mGraphicsPipeline->setScissors({scissor});

  // 2.5.Rasterizer.
  // The rasterizer takes the geometry that is shaped by the vertices from the
  // vertex shader and turns it into fragments to be colored by the fragment
  // shader.
  mGraphicsPipeline->mRasterizer.depthClampEnable = VK_FALSE;
  mGraphicsPipeline->mRasterizer.rasterizerDiscardEnable = VK_FALSE;

  mGraphicsPipeline->mRasterizer.polygonMode = VK_POLYGON_MODE_FILL;
  mGraphicsPipeline->mRasterizer.lineWidth = 1.0f;
  mGraphicsPipeline->mRasterizer.cullMode = VK_CULL_MODE_NONE;
  mGraphicsPipeline->mRasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;

  mGraphicsPipeline->mRasterizer.depthBiasEnable = VK_FALSE;
  mGraphicsPipeline->mRasterizer.depthBiasConstantFactor = 0.0f;
  mGraphicsPipeline->mRasterizer.depthBiasClamp = 0.0f;
  mGraphicsPipeline->mRasterizer.depthBiasSlopeFactor = 0.0f;

  // 2.6.Multi-sampling.
  // Multi-sampling is an anti-aliasing method used to make edges look less
  // jagged.
  mGraphicsPipeline->mMultisampling.sampleShadingEnable = VK_FALSE;
  mGraphicsPipeline->mMultisampling.rasterizationSamples =
      VK_SAMPLE_COUNT_1_BIT;
  mGraphicsPipeline->mMultisampling.minSampleShading = 1.0f;
  mGraphicsPipeline->mMultisampling.pSampleMask = nullptr;
  mGraphicsPipeline->mMultisampling.alphaToCoverageEnable = VK_FALSE;
  mGraphicsPipeline->mMultisampling.alphaToOneEnable = VK_FALSE;

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

  mGraphicsPipeline->addBlendAttachment(colorBlendAttachment);

  // 2.8.2 'VkPipelineColorBlendStateCreateInfo' contains the global color
  // blending settings
  //
  // if logicIpEnable == VK_TRUE, the 'VkPipelineColorBlendAttachmentState' will
  // be banned, but 'colorWriteMask' is valid.
  mGraphicsPipeline->mColorBlending.logicOpEnable =
      VK_FALSE;  // If usr the bitwise combination method of blending.
  mGraphicsPipeline->mColorBlending.logicOp =
      VK_LOGIC_OP_COPY;  // If specify the bitwise operation.
  mGraphicsPipeline->mColorBlending.blendConstants[0] = 0.0f;
  mGraphicsPipeline->mColorBlending.blendConstants[1] = 0.0f;
  mGraphicsPipeline->mColorBlending.blendConstants[2] = 0.0f;
  mGraphicsPipeline->mColorBlending.blendConstants[3] = 0.0f;

  // 3.Pipeline layout: the uniform and push values referenced by the shader
  // that can be updated at draw time
  mGraphicsPipeline->mPipelineLayoutInfo.setLayoutCount = 0;     // Optional
  mGraphicsPipeline->mPipelineLayoutInfo.pSetLayouts = nullptr;  // Optional
  mGraphicsPipeline->mPipelineLayoutInfo.pushConstantRangeCount =
      0;  // Optional
  mGraphicsPipeline->mPipelineLayoutInfo.pPushConstantRanges =
      nullptr;  // Optional

  // 4.Create pipeline
  mGraphicsPipeline->buildPipeline();
}

void HelloTriangleApplication::MainLoop() {
  while (!mWindow->windowShouldClose()) {
    mWindow->pollEvents();
    DrawFrame();
  }

  vkDeviceWaitIdle(mDevice->getDevice());
}

void HelloTriangleApplication::DrawFrame() {
  mFences[mCurrentFrame]->block();

  // Acquiring an image from the swap chain.
  uint32_t imageIndex{0};
  vkAcquireNextImageKHR(
      mDevice->getDevice(), mSwapChain->getSwapChain(), UINT64_MAX,
      mImageAvailableSemaphores[mCurrentFrame]->getSemaphore(), VK_NULL_HANDLE,
      &imageIndex);

  // Submitting the command buffer.
  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkSemaphore waitSemaphores[] = {
      mImageAvailableSemaphores[mCurrentFrame]->getSemaphore()};
  VkPipelineStageFlags waitStages[] = {
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSemaphores;
  submitInfo.pWaitDstStageMask = waitStages;

  auto commandBuffer = mCommandBuffers[imageIndex]->getCommandBuffer();
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffer;

  VkSemaphore signalSemaphores[] = {
      mRenderFinishedSemaphores[mCurrentFrame]->getSemaphore()};
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = signalSemaphores;

  mFences[mCurrentFrame]->resetFence();
  if (vkQueueSubmit(mDevice->getGraphicsQueue(), 1, &submitInfo,
                    mFences[mCurrentFrame]->getFence()) != VK_SUCCESS) {
    throw std::runtime_error("failed to submit draw command buffer!");
  }

  // Presentation
  VkPresentInfoKHR presentInfo{};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = signalSemaphores;

  VkSwapchainKHR swapChains[] = {mSwapChain->getSwapChain()};
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapChains;

  presentInfo.pImageIndices = &imageIndex;

  vkQueuePresentKHR(mDevice->getPresentQueue(), &presentInfo);

  // update current frame index
  mCurrentFrame = (mCurrentFrame + 1) % mSwapChain->getSwapChainImageCount();
}

void HelloTriangleApplication::CleanUp() {
  mGraphicsPipeline.reset();
  mSwapChain.reset();
  mDevice.reset();
  mSurface.reset();
  mInstance.reset();
  mWindow.reset();
}
