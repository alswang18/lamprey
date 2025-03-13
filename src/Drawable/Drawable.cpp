#include "Drawable.h"
#include "Bindable/Bindable.h"
#include "Bindable/IndexBuffer.h"
#include "Macros/LampreyAsserts.h"
#include <cassert>
#include <sys/stat.h>

void Drawable::Draw(Graphics& gfx) const
{
    for (const std::unique_ptr<Bindable>& b : binds)
    {
        b->Bind(gfx);
    }

    for (auto& b : GetStaticBinds())
    {
        b->Bind(gfx);
    }

    gfx.DrawIndexed(pIndexBuffer->GetCount());
}

void Drawable::AddBind(std::unique_ptr<Bindable> bind)
{
    LMPASSERT(typeid(bind.get()) != typeid(IndexBuffer));
    binds.push_back(std::move(bind));
}

void Drawable::AddIndexBuffer(
    std::unique_ptr<IndexBuffer> ibuf)
{
    LMPASSERT(pIndexBuffer == nullptr);
    pIndexBuffer = ibuf.get();

    std::unique_ptr<Bindable> bind(
        dynamic_cast<Bindable*>(ibuf.release()));

    binds.push_back(std::move(bind));
}