#pragma once
#include "Bindable/BindableCommon.h"
#include "DrawableBase.h"
#include "Vertex.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

class Mesh : public DrawableBase<Mesh>
{
public:
    Mesh(Graphics& gfx,
         std::vector<std::unique_ptr<Bind::Bindable>>
             bindPtrs);
    void Draw(
        Graphics& gfx,
        DirectX::FXMMATRIX accumulatedTransform) const;
    DirectX::XMMATRIX GetTransformXM()
        const noexcept override;

private:
    mutable DirectX::XMFLOAT4X4 transform;
};

class Node
{
    friend class Model;

public:
    Node(std::vector<Mesh*> meshPtrs,
         const DirectX::XMMATRIX& transform);
    void Draw(
        Graphics& gfx,
        DirectX::FXMMATRIX accumulatedTransform) const;

private:
    void AddChild(std::unique_ptr<Node> pChild);

private:
    std::vector<std::unique_ptr<Node>> childPtrs;
    std::vector<Mesh*> meshPtrs;
    DirectX::XMFLOAT4X4 transform;
};

class Model
{
public:
    Model(Graphics& gfx, const std::string fileName);
    void Draw(Graphics& gfx,
              DirectX::FXMMATRIX transform) const;

private:
    static std::unique_ptr<Mesh> ParseMesh(
        Graphics& gfx, const aiMesh& mesh);
    std::unique_ptr<Node> ParseNode(const aiNode& node);

private:
    std::unique_ptr<Node> pRoot;
    std::vector<std::unique_ptr<Mesh>> meshPtrs;
};
