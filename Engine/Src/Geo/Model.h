#pragma once
#include "Mesh.h"
#include "../Global/GlobalFuncs.h"

class Model final{ //A model obj acts as a container for several mesh objs
public:
    Model(cstr const& fPath, const std::initializer_list<aiTextureType>& iL);
    ~Model();
    void BatchRender(const int& primitive);
    void Render(ShaderProg& SP, const glm::mat4& PV, const int& primitive);
private:
    bool storeIndices;
    cstr modelPath;
    mutable std::vector<Mesh> meshes;
    mutable std::vector<aiTextureType> texTypes;
    std::vector<Vertex> allVertices;
    std::vector<uint> allIndices;
    uint VAO;
    uint VBO;
    uint EBO;

    void LoadModel() const;
    void ProcessNode(const aiScene* const& scene, const aiNode* const& node) const;
    Mesh ProcessMesh(const aiScene* const& scene, const aiMesh* const& meshObj) const;
};