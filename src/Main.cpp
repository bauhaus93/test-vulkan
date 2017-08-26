#include <iostream>
#include <stdexcept>

#include "logging/StdLogger.h"
#include "TestApp.h"

int main(int argc, char** argv) {
  TestApp app { 800, 600 };

    try {
        app.Run();
    }
    catch(const std::runtime_error& e) {
        ERROR(e.what());
        return 1;
    }

    return 0;
}
