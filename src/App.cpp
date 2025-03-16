#include "App.h"
#include "Drawable/Box.h"
#include "Drawable/Melon.h"
#include "Drawable/Pyramid.h"
#include "Drawable/Sheet.h"
#include "Drawable/SkinnedBox.h"
#include "Drawable/Surface.h"
#include "LampreyMath.h"
#include <algorithm>
#include <fmt/core.h>
#include <memory>

#include "GDIPlusManager.h"
#include "imgui.h"

GDIPlusManager gdipm;

class Factory
{
public:
    Factory(Graphics& gfx) : gfx(gfx) {}
    std::unique_ptr<Drawable> operator()()
    {
        const DirectX::XMFLOAT3 mat = {
            cdist(rng), cdist(rng), cdist(rng)};

        return std::make_unique<Box>(gfx, rng, adist, ddist,
                                     odist, rdist, bdist,
                                     mat);
    }

private:
    Graphics& gfx;
    std::mt19937 rng{std::random_device{}()};
    std::uniform_real_distribution<float> adist{0.0f,
                                                PI * 2.0f};
    std::uniform_real_distribution<float> ddist{0.0f,
                                                PI * 0.5f};
    std::uniform_real_distribution<float> odist{0.0f,
                                                PI * 0.08f};
    std::uniform_real_distribution<float> rdist{6.0f,
                                                20.0f};
    std::uniform_real_distribution<float> cdist{0.0f, 1.0f};
    std::uniform_real_distribution<float> bdist{0.4f, 3.0f};
};

App::App()
    : wnd(800, 600, "Lamprey Client"), light(wnd.Gfx())
{

    drawables.reserve(nDrawables);
    std::generate_n(std::back_inserter(drawables),
                    nDrawables, Factory{wnd.Gfx()});

    wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(
        1.0f, 3.0f / 4.0f, 0.5f, 40.0f));
}

int App::Go()
{
    while (true)
    {
        const std::optional<int> ecode =
            Window::ProcessMessages();

        if (ecode.has_value())
        {
            return ecode.value();
        }

        DoFrame();
    }
}

void App::DoFrame()
{
    const auto dt = timer.Mark() * speed_factor;
    wnd.Gfx().BeginFrame(0.07f, 0.0f, 0.12f);
    wnd.Gfx().SetCamera(cam.GetMatrix());
    light.Bind(wnd.Gfx());
    for (auto& d : drawables)
    {
        d->Update(wnd.keyboard.KeyIsPressed(VK_SPACE) ? 0.0f
                                                      : dt);
        d->Draw(wnd.Gfx());
    }
    light.Draw(wnd.Gfx());
    // imgui window to control simulation speed
    if (ImGui::Begin("Simulation Speed"))
    {
        ImGui::SliderFloat("Speed Factor", &speed_factor,
                           0.0f, 10.0f);
        ImGui::Text("%.3f ms/frame (%.1f FPS)",
                    1000.0f / ImGui::GetIO().Framerate,
                    ImGui::GetIO().Framerate);
        ImGui::Text(
            "Status: %s",
            wnd.keyboard.KeyIsPressed(VK_SPACE)
                ? "PAUSED"
                : "RUNNING (hold spacebar to pause)");
    }
    ImGui::End();

    cam.SpawnControlWindow();
    light.SpawnControlWindow();
    wnd.Gfx().EndFrame();
}

App::~App() {}