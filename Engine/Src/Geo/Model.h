#pragma once

#include "Mesh.h"
#include "../Global/GlobalFuncs.h"

class Model final{ //A model obj acts as a container for several mesh objs
public:
    Model();
    Model(cstr const& fPath, const std::initializer_list<aiTextureType>& iL);
    ~Model();

    void AddModelMat4ForAll(const glm::mat4& modelMat4);
    void ClearModelMat4VecForAll();
    void InstancedRender(ShaderProg& SP, const int& primitive = GL_TRIANGLES);
    void Render(ShaderProg& SP, const int& primitive = GL_TRIANGLES);
    void SetModelMat4ForAll(const glm::mat4& modelMat4);
private:
    bool storeIndices;
    cstr modelPath;
    mutable std::vector<Mesh> meshes;
    mutable std::vector<aiTextureType> texTypes;
    std::vector<Vertex> allVertices;
    std::vector<uint> allIndices;
    glm::mat4 modelMat4ForAll;
    std::vector<glm::mat4> modelMat4VecForAll;
    uint VAO;
    uint VBO;
    uint EBO;

    void LoadModel() const;
    void ProcessNode(const aiScene* const& scene, const aiNode* const& node) const;
    Mesh ProcessMesh(const aiScene* const& scene, const aiMesh* const& meshObj) const;
};