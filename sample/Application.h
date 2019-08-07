#ifndef APPLICATION_H
#define APPLICATION_H

#include <GLFW/glfw3.h>
#include <hairgl/HairGL.h>
#include "CameraController.h"

class Application
{
public:
    Application(const char* windowTitle, int initialScreenWidth, int initialScreenHeight);
    void Run();
    ~Application();

private:
    GLFWwindow* window;
    HairGL::HairSystem* hairSystem;
    HairGL::HairAsset* hairAsset;
    HairGL::HairInstance* hairInstance;
    HairGL::HairInstanceSettings hairSettings;
    CameraController* cameraController;

    void Render();
    void Update(float timeStep);
};

#endif