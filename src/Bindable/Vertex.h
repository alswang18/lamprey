#pragma once
#include "Color.h"
#include "Graphics.h"
#include <type_traits>
#include <vector>

namespace Dvtx
{
class VertexLayout
{
public:
    enum ElementType
    {
        Position2D,
        Position3D,
        Texture2D,
        Normal,
        Float3Color,
        Float4Color,
        BGRAColor,
        Count,
    };
    template<ElementType>
    struct Map;
    template<>
    struct Map<Position2D>
    {
        using SysType = DirectX::XMFLOAT2;
        static constexpr DXGI_FORMAT dxgiFormat =
            DXGI_FORMAT_R32G32_FLOAT;
        static constexpr const char* semantic = "Position";
    };
    template<>
    struct Map<Position3D>
    {
        using SysType = DirectX::XMFLOAT3;
        static constexpr DXGI_FORMAT dxgiFormat =
            DXGI_FORMAT_R32G32B32_FLOAT;
        static constexpr const char* semantic = "Position";
    };
    template<>
    struct Map<Texture2D>
    {
        using SysType = DirectX::XMFLOAT2;
        static constexpr DXGI_FORMAT dxgiFormat =
            DXGI_FORMAT_R32G32_FLOAT;
        static constexpr const char* semantic = "Texcoord";
    };
    template<>
    struct Map<Normal>
    {
        using SysType = DirectX::XMFLOAT3;
        static constexpr DXGI_FORMAT dxgiFormat =
            DXGI_FORMAT_R32G32B32_FLOAT;
        static constexpr const char* semantic = "Normal";
    };
    template<>
    struct Map<Float3Color>
    {
        using SysType = DirectX::XMFLOAT3;
        static constexpr DXGI_FORMAT dxgiFormat =
            DXGI_FORMAT_R32G32B32_FLOAT;
        static constexpr const char* semantic = "Color";
    };
    template<>
    struct Map<Float4Color>
    {
        using SysType = DirectX::XMFLOAT4;
        static constexpr DXGI_FORMAT dxgiFormat =
            DXGI_FORMAT_R32G32B32A32_FLOAT;
        static constexpr const char* semantic = "Color";
    };
    template<>
    struct Map<BGRAColor>
    {
        using SysType = ::BGRAColor;
        static constexpr DXGI_FORMAT dxgiFormat =
            DXGI_FORMAT_R8G8B8A8_UNORM;
        static constexpr const char* semantic = "Color";
    };

    class Element
    {
    public:
        Element(ElementType type, size_t offset);
        size_t GetOffsetAfter() const;
        size_t GetOffset() const;
        size_t Size() const;
        static constexpr size_t SizeOf(ElementType type);
        ElementType GetType() const noexcept;
        D3D11_INPUT_ELEMENT_DESC GetDesc() const;

    private:
        template<ElementType type>
        static constexpr D3D11_INPUT_ELEMENT_DESC
        GenerateDesc(size_t offset)
        {
            return {Map<type>::semantic,
                    0,
                    Map<type>::dxgiFormat,
                    0,
                    (UINT)offset,
                    D3D11_INPUT_PER_VERTEX_DATA,
                    0};
        }

    private:
        ElementType type;
        size_t offset;
    };

public:
    template<ElementType Type>
    const Element& Resolve() const
    {
        for (auto& e : elements)
        {
            if (e.GetType() == Type)
            {
                return e;
            }
        }
        assert("Could not resolve element type" && false);
        return elements.front();
    }
    const Element& ResolveByIndex(size_t i) const;
    VertexLayout& Append(ElementType type);
    size_t Size() const;
    size_t GetElementCount() const noexcept;
    std::vector<D3D11_INPUT_ELEMENT_DESC> GetD3DLayout()
        const;

private:
    std::vector<Element> elements;
};

class Vertex
{
    friend class VertexBuffer;

public:
    template<VertexLayout::ElementType Type>
    auto& Attr()
    {
        auto pAttribute =
            pData + layout.Resolve<Type>().GetOffset();
        return *reinterpret_cast<
            typename VertexLayout::Map<Type>::SysType*>(
            pAttribute);
    }
    template<typename T>
    void SetAttributeByIndex(size_t i, T&& val)
    {
        const auto& element = layout.ResolveByIndex(i);
        auto pAttribute = pData + element.GetOffset();
        switch (element.GetType())
        {
        case VertexLayout::Position2D:
            SetAttribute<VertexLayout::Position2D>(
                pAttribute, std::forward<T>(val));
            break;
        case VertexLayout::Position3D:
            SetAttribute<VertexLayout::Position3D>(
                pAttribute, std::forward<T>(val));
            break;
        case VertexLayout::Texture2D:
            SetAttribute<VertexLayout::Texture2D>(
                pAttribute, std::forward<T>(val));
            break;
        case VertexLayout::Normal:
            SetAttribute<VertexLayout::Normal>(
                pAttribute, std::forward<T>(val));
            break;
        case VertexLayout::Float3Color:
            SetAttribute<VertexLayout::Float3Color>(
                pAttribute, std::forward<T>(val));
            break;
        case VertexLayout::Float4Color:
            SetAttribute<VertexLayout::Float4Color>(
                pAttribute, std::forward<T>(val));
            break;
        case VertexLayout::BGRAColor:
            SetAttribute<VertexLayout::BGRAColor>(
                pAttribute, std::forward<T>(val));
            break;
        default:
            assert("Bad element type" && false);
        }
    }

protected:
    Vertex(char* pData, const VertexLayout& layout);

private:
    // enables parameter pack setting of multiple parameters
    // by element index
    template<typename First, typename... Rest>
    void SetAttributeByIndex(size_t i, First&& first,
                             Rest&&... rest)
    {
        SetAttributeByIndex(i, std::forward<First>(first));
        SetAttributeByIndex(i + 1,
                            std::forward<Rest>(rest)...);
    }
    // helper to reduce code duplication in
    // SetAttributeByIndex
    template<VertexLayout::ElementType DestLayoutType,
             typename SrcType>
    void SetAttribute(char* pAttribute, SrcType&& val)
    {
        using Dest = typename VertexLayout::Map<
            DestLayoutType>::SysType;
        if constexpr (std::is_assignable<Dest,
                                         SrcType>::value)
        {
            *reinterpret_cast<Dest*>(pAttribute) = val;
        }
        else
        {
            assert("Parameter attribute type mismatch" &&
                   false);
        }
    }

private:
    char* pData = nullptr;
    const VertexLayout& layout;
};

class ConstVertex
{
public:
    ConstVertex(const Vertex& v);
    template<VertexLayout::ElementType Type>
    const auto& Attr() const
    {
        return const_cast<Vertex&>(vertex).Attr<Type>();
    }

private:
    Vertex vertex;
};

class VertexBuffer
{
public:
    VertexBuffer(VertexLayout layout);
    const char* GetData() const;
    const VertexLayout& GetLayout() const noexcept;
    size_t Size() const;
    size_t SizeBytes() const;
    template<typename... Params>
    void EmplaceBack(Params&&... params)
    {
        assert(sizeof...(params) ==
                   layout.GetElementCount() &&
               "Param count doesn't match number of vertex "
               "elements");
        buffer.resize(buffer.size() + layout.Size());
        Back().SetAttributeByIndex(
            0u, std::forward<Params>(params)...);
    }
    Vertex Back();
    Vertex Front();
    Vertex operator[](size_t i);
    ConstVertex Back() const;
    ConstVertex Front() const;
    ConstVertex operator[](size_t i) const;

private:
    std::vector<char> buffer;
    VertexLayout layout;
};
} // namespace Dvtx