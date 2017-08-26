#pragma once

#include <stdexcept>
#include <vector>
#include <iostream>
#include <cstring>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include "utility/Extension.h"
#include "utility/ValidationLayer.h"
#include "utility/StringFormat.h"
#include "logging/StdLogger.h"
#include "DeviceCandidate.h"

class TestApp {

public:

          TestApp(int width, int height);
          ~TestApp();

  void    Run();


private:

  GLFWwindow*               window;
  VkInstance                instance;
  VkDebugReportCallbackEXT  debugCallback;
  VkSurfaceKHR              surface;
  VkPhysicalDevice          physicalDevice;
  VkDevice                  logicalDevice;
  VkQueue                   graphicsQueue;

  void          CreateWindow(int width, int height);
  void          InitVulkan();
  void          CreateInstance();
  void          SetupDebugCallback();
  void          CreateSurface();
  void          PickPhysicalDevice();
  void          Mainloop();

};
