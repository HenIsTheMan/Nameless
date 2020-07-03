#pragma once
#include "../Core.h"
#include "Vertex.h"

class Mesh{ //Single drawable geo
public:
	enum struct MeshType{ //Can be extended
		None = -1,
		Pt = 0,
		Line = 1,
		Quad = 2,
		Cuboid = 3,
	};
	Mesh();
	Mesh(const MeshType& myType, const int& myPrimitive);
	virtual ~Mesh();
	void Update(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection);
	void Render();
protected:
	MeshType type;
	int primitive;
	std::vector<Vertex>* vertices;
	std::vector<uint>* indices;
	uint VAO;
	uint VBO;
	uint EBO;
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;
	void CreateQuad();
};

//texIDs??
//UpdateMat(...) and UpdateType(...)