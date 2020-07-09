#include "Model.h"

Model::Model(cstr const& fPath, const std::initializer_list<aiTextureType>& iL):
    modelPath(fPath),
    meshes{},
    texTypes(iL)
{
}

void Model::LoadModel() const{ //Load model into a DS of Assimp called a scene obj (root obj of Assimp's data interface)
    Assimp::Importer importer;
    const aiScene* const scene = importer.ReadFile(modelPath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    if(!scene || !scene->mRootNode || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE){ //If !scene || !(root node of scene) || returned data is incomplete (given by 1 of its flags)
        printf("Assimp error: %s\n", importer.GetErrorString());
        return;
    }
    ProcessNode(scene, scene->mRootNode);
}

void Model::ProcessNode(const aiScene* const& scene, const aiNode* const& node) const{ //Process all of the scene obj's nodes recursively to translate the loaded data to an arr of Mesh objs //For parent-child relation between meshes
    for(uint i = 0; i < node->mNumMeshes; ++i){ //Process all the nodes' meshes (if any)
        const aiMesh* const mesh = scene->mMeshes[node->mMeshes[i]]; //Check a node's mesh indices and retrieve the corresponding mesh by indexing the scene's mMeshes array
        meshes.emplace_back(ProcessMesh(scene, mesh)); //Store mesh obj
    }
    for(uint i = 0; i < node->mNumChildren; ++i){ //node->mNumChildren is base case/exit condition/terminating condition
        ProcessNode(scene, node->mChildren[i]); //Each node contains a set of mesh indices where each points to a specific mesh located in the scene obj
    }
}

Mesh Model::ProcessMesh(const aiScene* const& scene, const aiMesh* const& meshObj) const{
    Mesh mesh;
    mesh.vertices = new std::vector<Vertex>();
    mesh.indices = new std::vector<uint>();

    for(uint i = 0; i < meshObj->mNumVertices; ++i){ //For each vertex of the mesh...
        mesh.vertices->push_back({
            glm::vec3(meshObj->mVertices[i].x, meshObj->mVertices[i].y, meshObj->mVertices[i].z),
            meshObj->mColors[0] ? glm::vec4(meshObj->mColors[0][i].r, meshObj->mColors[0][i].g, meshObj->mColors[0][i].b, meshObj->mColors[0][i].a) : glm::vec4(0.f),
            meshObj->mTextureCoords[0] ? glm::vec2(meshObj->mTextureCoords[0][i].x, meshObj->mTextureCoords[0][i].y) : glm::vec2(0.f),
            meshObj->mNormals ? glm::vec3(meshObj->mNormals[i].x, meshObj->mNormals[i].y, meshObj->mNormals[i].z) : glm::vec3(0.f),
            meshObj->mTangents ? glm::vec3(meshObj->mTangents[i].x, meshObj->mTangents[i].y, meshObj->mTangents[i].z) : glm::vec3(0.f),
        });
    }
    for(uint i = 0; i < meshObj->mNumFaces; ++i){ //For each face of the mesh... //Each mesh has an arr of primitive faces (triangles due to the aiProcess_Triangulate post-processing option)
        for(uint j = 0; j < meshObj->mFaces[i].mNumIndices; ++j){
            mesh.indices->emplace_back(meshObj->mFaces[i].mIndices[j]);
        }
    }
    if(meshObj->mMaterialIndex >= 0){ //Query the mesh's mtl index to check if the mesh contains a mtl
        LoadMtlTexs(scene->mMaterials[meshObj->mMaterialIndex]); //scene->mMaterials[meshObj->mMaterialIndex] is mtlObj
    }
    return mesh;
}

void Model::LoadMtlTexs(const aiMaterial* const& mtl) const{ //Helper func to retrieve texs from mtl
    for(size_t i = 0; i < texTypes.size(); ++i){
        for(uint j = 0; j < mtl->GetTextureCount(texTypes[i]); ++j){
            aiString aiStr;
            mtl->GetTexture(texTypes[i], j, &aiStr);
            texRefIDs.emplace_back();
            SetUpTex({
                ("Client/Imgs" + str(aiStr.C_Str())).c_str(),
                false, //No need to flip tex as aiProcess_FlipUVs flag is set
                GL_TEXTURE_2D,
                GL_REPEAT,
                GL_LINEAR_MIPMAP_LINEAR,
                GL_LINEAR,
            }, texRefIDs[i]);
        }
    }
}

void Model::Render(const int& primitive){
    if(!meshes.size()){
        LoadModel();
    }
}