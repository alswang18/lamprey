#pragma once
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
    Window wnd;
    Timer timer;
    std::vector<std::unique_ptr<class Drawable>> drawables;
    static constexpr size_t nDrawables = 180;
};