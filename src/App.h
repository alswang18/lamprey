#pragma once
#include "Camera.h"
#include "ImguiManager.h"
#include "Mesh.h"
#include "PointLight.h"
#include "Timer.h"
#include "Window.h"

class App
{
public:
    App();
    ~App();
    // master frame / message loop
    int Go();

private:
    void DoFrame();
    void ShowModelWindow();

private:
    ImguiManager imgui;
    Window wnd;
    Timer timer;
    float speed_factor = 1.0f;
    Camera cam;
    PointLight light;
    Model nano{wnd.Gfx(), "nanosuit.obj.model"};
    struct
    {
        float roll = 0.0f;
        float pitch = 0.0f;
        float yaw = 0.0f;
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
    } pos;
};