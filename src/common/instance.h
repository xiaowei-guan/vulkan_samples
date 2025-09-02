#ifndef INSTANCE_H_
#define INSTANCE_H_

#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

class Instance {
 public:
  using Ptr = std::shared_ptr<Instance>;
  static Ptr create(bool enableValidationLayer) {
    return std::make_shared<Instance>(enableValidationLayer);
  }

  Instance(bool enableValidationLayer);

  ~Instance();

  void checkExtensionSupport();
  std::vector<const char *> getRequiredExtensions();

  bool checkValidationLayerSupport();
  void setupDebugger();

  [[nodiscard]] VkInstance getInstance() const { return mInstance; }
  [[nodiscard]] bool isValidationLayerEnabled() const {
    return mEnableValidationLayer;
  }

 private:
  VkInstance mInstance{VK_NULL_HANDLE};
  bool mEnableValidationLayer{false};
  VkDebugUtilsMessengerEXT mDebugger{VK_NULL_HANDLE};
};

#endif
