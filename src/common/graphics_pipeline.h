#ifndef GRAPHICS_PIPELINE_H_
#define GRAPHICS_PIPELINE_H_


#include "device.h"
#include "render_pass.h"
#include <string>



class ShaderStageInfo {
public:
  using Ptr = std::shared_ptr<ShaderStageInfo>;
  static Ptr create(const Device::Ptr &device, const std::string &fileName,
                    VkShaderStageFlagBits shaderStage,
                    const std::string &entryPoint) {
    return std::make_shared<ShaderStageInfo>(device, fileName, shaderStage,
                                             entryPoint);
  }

  ShaderStageInfo(const Device::Ptr &device, const std::string &fileName,
                  VkShaderStageFlagBits shaderStage,
                  const std::string &entryPoint);

  ~ShaderStageInfo();

  [[nodiscard]] auto getShaderStage() const { return mShaderStage; }
  [[nodiscard]] auto &getShaderEntryPoint() const { return mEntryPoint; }
  [[nodiscard]] auto getShaderModule() const { return mShaderModule; }
  [[nodiscard]] auto getShaderStageInfo() const { return mShaderStageInfo; }

private:
  void createShaderModule(const std::vector<char> &code);

private:
  VkShaderModule mShaderModule{VK_NULL_HANDLE};
  VkPipelineShaderStageCreateInfo mShaderStageInfo{};

  Device::Ptr mDevice{nullptr};
  std::string mEntryPoint;

  VkShaderStageFlagBits mShaderStage;
};

class GraphicsPipeline {
public:
  using Ptr = std::shared_ptr<GraphicsPipeline>;
  static Ptr create(const Device::Ptr &device,
                    const RenderPass::Ptr &renderPass) {
    return std::make_shared<GraphicsPipeline>(device, renderPass);
  }

  GraphicsPipeline(const Device::Ptr &device,
                   const RenderPass::Ptr &renderPass);
  ~GraphicsPipeline();

  void
  setShaderStages(std::vector<VkPipelineShaderStageCreateInfo> &shaderStages) {
    mShaderStages = shaderStages;
  }

  void setViewports(const std::vector<VkViewport> &viewports) {
    mViewports = viewports;
  }

  void setScissors(const std::vector<VkRect2D> &scissors) {
    mScissors = scissors;
  }

  void addBlendAttachment(
      const VkPipelineColorBlendAttachmentState &blendAttachment) {
    mBlendAttachmentStates.push_back(blendAttachment);
  }

  void buildPipeline();

  [[nodiscard]] auto getPipelineLayout() const { return mPipelineLayout; }
  [[nodiscard]] auto getPipeline() const { return mGraphicsPipeline; }

public:
  VkPipelineVertexInputStateCreateInfo mVertexInputInfo{};
  VkPipelineInputAssemblyStateCreateInfo mInputAssembly{};
  VkPipelineViewportStateCreateInfo mViewportState{};
  VkPipelineRasterizationStateCreateInfo mRasterizer{};
  VkPipelineMultisampleStateCreateInfo mMultisampling{};

  std::vector<VkPipelineColorBlendAttachmentState> mBlendAttachmentStates{};
  VkPipelineColorBlendStateCreateInfo mColorBlending{};

  VkPipelineLayoutCreateInfo mPipelineLayoutInfo{};

private:
  VkPipelineLayout mPipelineLayout{VK_NULL_HANDLE};
  VkPipeline mGraphicsPipeline{VK_NULL_HANDLE};

  Device::Ptr mDevice{nullptr};
  RenderPass::Ptr mRenderPass{nullptr};

  std::vector<VkPipelineShaderStageCreateInfo> mShaderStages;
  std::vector<VkViewport> mViewports{};
  std::vector<VkRect2D> mScissors{};
};

#endif
