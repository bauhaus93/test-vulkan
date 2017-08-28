#include "TestApp.h"

TestApp::TestApp(int width, int height):
    engine { width, height } {

}

TestApp::~TestApp() {
}

void TestApp::Run() {
    Mainloop();
}

void TestApp::Mainloop() {
    while (!engine.ShouldQuit()) {
        engine.HandleEvents();
    }
}
