#pragma once
#include "../Core.h"

struct Vertex final{
    Vertex():
        pos(0.f),
        colour(0.f),
        texCoords(0.f),
        normal(0.f),
        tangent(0.f),
        process(0.f)
    {
    }
    Vertex(const glm::vec3& myPos, const glm::vec4& myColour, const glm::vec2& myTexCoords, const glm::vec3& myNormal, const glm::vec3& myTangent, const float& myProcess):
        pos(myPos),
        colour(myColour),
        texCoords(myTexCoords),
        normal(myNormal),
        tangent(myTangent),
        process(myProcess)
    {
    }
	glm::vec3 pos;
	glm::vec4 colour;
    glm::vec2 texCoords;
    glm::vec3 normal;
    glm::vec3 tangent;
    float process;
};