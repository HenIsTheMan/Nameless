#pragma once
#include "../Core.h"

struct Vertex final{
	glm::vec3 pos;
	glm::vec4 colour;
    glm::vec2 texCoords;
    glm::vec3 normal;
    glm::vec3 tangent;
    int diffuseTexIndex = 0;
};