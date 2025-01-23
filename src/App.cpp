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
    const float timeElapsed = wnd.timer.Peek();
    std::string fps = fmt::format("Time Elapsed {}s", timeElapsed);
    wnd.SetTitle(fps);
}