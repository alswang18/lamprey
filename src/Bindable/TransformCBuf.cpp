#include "TransformCbuf.h"

std::unique_ptr<
    VertexConstantBuffer<TransformCbuf::Transforms>>
    TransformCbuf::pVcbuf;

TransformCbuf::TransformCbuf(Graphics& gfx,
                             const Drawable& parent,
                             UINT slot)
    : parent(parent)
{
    if (!pVcbuf)
    {
        pVcbuf = std::make_unique<
            VertexConstantBuffer<Transforms>>(gfx, slot);
    }
}

void TransformCbuf::Bind(Graphics& gfx) noexcept
{
    const auto model = parent.GetTransformXM();
    const Transforms tf = {
        DirectX::XMMatrixTranspose(model),
        DirectX::XMMatrixTranspose(model * gfx.GetCamera() *
                                   gfx.GetProjection())};
    pVcbuf->Update(gfx, tf);
    pVcbuf->Bind(gfx);
}
