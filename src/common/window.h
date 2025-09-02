#ifndef WINDOW_H_
#define WINDOW_H_

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <memory>

class Window {
 public:
  using Ptr = std::shared_ptr<Window>;
  static Ptr create(const uint32_t &width, const uint32_t &height) {
    return std::make_shared<Window>(width, height);
  }

  Window(const uint32_t &width, const uint32_t &height);

  ~Window();

  bool windowShouldClose();

  void pollEvents();

  [[nodiscard]] auto getWindow() const { return mWindow; }

 private:
  uint32_t mWidth{0};
  uint32_t mHeight{0};
  GLFWwindow *mWindow{NULL};
};

#endif
