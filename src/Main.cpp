#include <iostream>
#include <stdexcept>

#include "TestApp.h"

int main(int argc, char** argv) {
  TestApp app { 800, 600 };

  try {
    app.Run();
  }
  catch(const std::runtime_error& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  return 0;
}
