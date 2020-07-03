#pragma once
#include "../Core.h"

struct Vertex final{
    Vertex() = delete;
    Vertex(const glm::vec3& myPos, const glm::vec4& myColour, const glm::vec2& myTexCoords, const std::vector<uint>& myTexIDs, const glm::vec3& myNormal, const glm::vec3& myTangent):
        pos(myPos),
        colour(myColour),
        texCoords(myTexCoords),
        texIDs(myTexIDs),
        normal(myNormal),
        tangent(myTangent)
    {
    }
	glm::vec3 pos;
	glm::vec4 colour;
    glm::vec2 texCoords;
    std::vector<uint> texIDs;
    glm::vec3 normal;
    glm::vec3 tangent;
};