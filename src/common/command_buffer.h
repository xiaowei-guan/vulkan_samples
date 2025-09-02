#ifndef COMMAND_BUFFER_H
#define COMMAND_BUFFER_H

#include "device.h"

class CommandPool {
 public:
  using Ptr = std::shared_ptr<CommandPool>;
  static Ptr create(const Device::Ptr &device,
                    VkCommandPoolCreateFlagBits flag =
                        VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT) {
    return std::make_shared<CommandPool>(device, flag);
  };

  CommandPool(const Device::Ptr &device,
              VkCommandPoolCreateFlagBits flag =
                  VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
  ~CommandPool();

  [[nodiscard]] auto getCommandPool() const { return mCommandPool; }

 private:
  VkCommandPool mCommandPool{VK_NULL_HANDLE};

  Device::Ptr mDevice{nullptr};
};

class CommandBuffer {
 public:
  using Ptr = std::shared_ptr<CommandBuffer>;
  static Ptr create(const Device::Ptr &device,
                    const CommandPool::Ptr &commandPool,
                    bool asSecondary = false) {
    return std::make_shared<CommandBuffer>(device, commandPool, asSecondary);
  };

  CommandBuffer(const Device::Ptr &device, const CommandPool::Ptr &commandPool,
                bool asSecondary = false);
  ~CommandBuffer();

  void begin(VkCommandBufferUsageFlags flag = 0,
             const VkCommandBufferInheritanceInfo &inheritance = {
                 .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO});
  void beginRenderPass(
      const VkRenderPassBeginInfo &renderPassBeginInfo,
      const VkSubpassContents &subPassContents = VK_SUBPASS_CONTENTS_INLINE);
  void bindGraphicPipeline(const VkPipeline &pipeline);
  void draw(size_t vertexCount);
  void endRenderPass();
  void end();

  [[nodiscard]] auto getCommandBuffer() const { return mCommandBuffer; }

 private:
  VkCommandBuffer mCommandBuffer{VK_NULL_HANDLE};

  Device::Ptr mDevice{nullptr};
  CommandPool::Ptr mCommandPool{nullptr};
};

#endif
