#pragma once

#include <stdexcept>
#include <vector>
#include <iostream>
#include <cstring>

#include "utility/StringFormat.h"
#include "logging/StdLogger.h"
#include "engine/Engine.h"

class TestApp {

public:

                    TestApp(int width, int height);
                    ~TestApp();

  void              Run();


private:
    engine::Engine  engine;

    void            Mainloop();

};
