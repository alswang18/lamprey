#include "App.h"
#include <fmt/core.h>

App::App() : wnd(800, 600, "Lamprey Client") {}

int App::Go()
{
    while (true)
    {
        const std::optional<int> ecode = Window::ProcessMessages();
        if (ecode.has_value())
        {
            return ecode.value();
            // if return optional has value, means we're quitting so return exit code
        }
        DoFrame();
    }
}

void App::DoFrame()
{
    const float c = sin(timer.Peek()) / 2.0f + 0.5f;
    wnd.Gfx().ClearBuffer(c, c, 1.0f);
    wnd.Gfx().EndFrame();
}