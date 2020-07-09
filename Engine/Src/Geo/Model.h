#pragma once
#include "Mesh.h"
#include "../Global/GlobalFuncs.h"

class Model final{ //A model obj acts as a container for several mesh objs
    cstr modelPath;
    mutable std::vector<Mesh> meshes;
    mutable std::vector<aiTextureType> texTypes;
    mutable std::vector<uint> texRefIDs;

    void LoadModel() const;
    void ProcessNode(const aiScene* const& scene, const aiNode* const& node) const;
    Mesh ProcessMesh(const aiScene* const& scene, const aiMesh* const& meshObj) const;
    void LoadMtlTexs(const aiMaterial* const& mtl) const;
public:
    Model(cstr const& fPath, const std::initializer_list<aiTextureType>& iL);
    void Render(const int& primitive);
};