#pragma once
#include "ConstantBuffers.h"
#include "Graphics.h"
#include "SolidSphere.h"

class PointLight
{
public:
    PointLight(Graphics& gfx, float radius = 0.5f);
    void SpawnControlWindow() noexcept;
    void Reset() noexcept;
    void Draw(Graphics& gfx);
    void Bind(Graphics& gfx) noexcept;

private:
    struct PointLightCBuf
    {
        DirectX::XMFLOAT3 pos;
        float padding;
    };

private:
    DirectX::XMFLOAT3 pos = {0.0f, 0.0f, 0.0f};
    ;
    SolidSphere mesh;
    PixelConstantBuffer<PointLightCBuf> cbuf;
};