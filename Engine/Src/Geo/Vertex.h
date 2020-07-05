#pragma once
#include "../Core.h"

struct Vertex final{
    Vertex():
        pos(0.f),
        colour(0.f),
        texCoords(0.f),
        texUnits{},
        normal(0.f),
        tangent(0.f)
    {
    }
    Vertex(const glm::vec3& myPos, const glm::vec4& myColour, const glm::vec2& myTexCoords, const std::vector<uint>& myTexUnits, const glm::vec3& myNormal, const glm::vec3& myTangent):
        pos(myPos),
        colour(myColour),
        texCoords(myTexCoords),
        texUnits(myTexUnits),
        normal(myNormal),
        tangent(myTangent)
    {
    }
	glm::vec3 pos;
	glm::vec4 colour;
    glm::vec2 texCoords;
    std::vector<uint> texUnits;
    glm::vec3 normal;
    glm::vec3 tangent;
};