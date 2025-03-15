#pragma once
#include "Camera.h"
#include "ImguiManager.h"
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

private:
    ImguiManager imgui;
    Window wnd;
    Timer timer;
    float speed_factor = 1.0f;
    Camera cam;
    std::vector<std::unique_ptr<class Drawable>> drawables;
    static constexpr size_t nDrawables = 180;
};