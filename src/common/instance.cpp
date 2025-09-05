////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License.  You may obtain a copy
// of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
// License for the specific language governing permissions and limitations
// under the License.
////////////////////////////////////////////////////////////////////////////////

#include "common/instance.h"

#include <GLFW/glfw3.h>
#include <string.h>

#include <iostream>
#include <string>

// Validation layers.
const std::vector<const char *> validationLayers = {
    "VK_LAYER_KHRONOS_validation"};

// Validation layer debug callback.
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallBack(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *pMessageData, void *pUserData) {
  std::cerr << "ValidationLayer: " << pMessageData->pMessage << std::endl;

  return VK_FALSE;
}

// vkCreateDebugUtilsMessengerEXT is an extension function. We need to load it
// dynamically.
static VkResult CreateDebugUtilsMessengerEXT(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
    const VkAllocationCallbacks *pAllocator,
    VkDebugUtilsMessengerEXT *debugMessenger) {
  auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      instance, "vkCreateDebugUtilsMessengerEXT");

  if (func != nullptr) {
    return func(instance, pCreateInfo, pAllocator, debugMessenger);
  } else {
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }
}
// vkDestroyDebugUtilsMessengerEXT is also an extension function. We need to
// load it dynamically.
static void DestroyDebugUtilsMessengerEXT(
    VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
    const VkAllocationCallbacks *pAllocator) {
  auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      instance, "vkDestroyDebugUtilsMessengerEXT");

  if (func != nullptr) {
    return func(instance, debugMessenger, pAllocator);
  }
}

Instance::Instance(bool enableValidationLayer) {
  mEnableValidationLayer = enableValidationLayer;

  if (mEnableValidationLayer && !checkValidationLayerSupport()) {
    throw std::runtime_error("Error: validation layer is not supported");
  }

  checkExtensionSupport();

  VkApplicationInfo appInfo = {};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = "Hello Triangle";
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "NO ENGINE";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_0;

  VkInstanceCreateInfo instCreateInfo = {};
  instCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instCreateInfo.pApplicationInfo = &appInfo;

  // Need extensions to interface with the window system.
  auto extensions = getRequiredExtensions();
  instCreateInfo.enabledExtensionCount =
      static_cast<uint32_t>(extensions.size());
  instCreateInfo.ppEnabledExtensionNames = extensions.data();

  // Set validation layer info.
  VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
  if (mEnableValidationLayer) {
    instCreateInfo.enabledLayerCount =
        static_cast<uint32_t>(validationLayers.size());
    instCreateInfo.ppEnabledLayerNames = validationLayers.data();

    // *******cause three i386 error, but not really a bug***********
    // *******The short story is that the Vulkan loader tries to load every
    // driver installed on the system so that it can report information about
    // each driver back to the application. In this case, vulkaninfo is a 64-bit
    // process, and attempting to load 32-bit drivers generates these
    // messages.************
    // // Good idea: It will automatically be used during vkCreateInstance(..)
    // and vkDestroyInstance and cleaned up after that.
    // populateDebugMessengerCreateInfo(debugCreateInfo);
    // instCreateInfo.pNext =
    // (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
  } else {
    instCreateInfo.enabledLayerCount = 0;
  }

  if (vkCreateInstance(&instCreateInfo, nullptr, &mInstance) != VK_SUCCESS) {
    throw std::runtime_error("Error:failed to create instance");
  }

  setupDebugger();
}

Instance::~Instance() {
  if (mEnableValidationLayer) {
    DestroyDebugUtilsMessengerEXT(mInstance, mDebugger, nullptr);
  }
  vkDestroyInstance(mInstance, nullptr);
}

void Instance::checkExtensionSupport() {
  uint32_t extensionCount = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

  std::vector<VkExtensionProperties> extensions(extensionCount);
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount,
                                         extensions.data());

  std::cout << "Available extensions:" << std::endl;

  for (const auto &extension : extensions) {
    std::cout << extension.extensionName << std::endl;
  }
}

std::vector<const char *> Instance::getRequiredExtensions() {
  uint32_t glfwExtensionCount = 0;

  const char **glfwExtensions =
      glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

  std::vector<const char *> extensions(glfwExtensions,
                                       glfwExtensions + glfwExtensionCount);

  if (mEnableValidationLayer) {
    // Set up a debug messenger with a callback using this extension.
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }

  return extensions;
}

bool Instance::checkValidationLayerSupport() {
  uint32_t layerCount = 0;
  vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

  std::vector<VkLayerProperties> availableLayers(layerCount);
  vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

  for (const auto &layerName : validationLayers) {
    bool layerFound = false;

    for (const auto &layerProp : availableLayers) {
      if (strcmp(layerName, layerProp.layerName) == 0) {
        layerFound = true;
        break;
      }
    }

    if (!layerFound) {
      return false;
    }
  }

  return true;
}

// If not set, the validation layers will print debug messages to the standard
// output by default. If set, we can handle debug messages in our program.
void Instance::setupDebugger() {
  if (!mEnableValidationLayer) {
    return;
  }

  VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  createInfo.pfnUserCallback = debugCallBack;
  createInfo.pUserData = nullptr;  // Optional

  if (CreateDebugUtilsMessengerEXT(mInstance, &createInfo, nullptr,
                                   &mDebugger) != VK_SUCCESS) {
    throw std::runtime_error("Error:failed to create debugger");
  }
}
