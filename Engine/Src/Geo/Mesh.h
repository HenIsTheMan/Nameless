#pragma once
#include "Vertex.h"

class Mesh{ //Single drawable geo
	friend class Model;
public:
	enum struct MeshType{
		//Pt, Line, 2D (circle, ring and 2D polygons with >= 3 vertices), 3D (sphere, torus and 3D polyhedrons with >= 3 flat polygonal faces)
		None = 0,
		Quad,
		Cuboid,
		Amt
	};
	struct BatchRenderParams final{
		glm::mat4 model;
		glm::vec4 colour;
		int texIndex;
	};
	Mesh();
	Mesh(const MeshType& myType, const int& myPrimitive);
	virtual ~Mesh();
	const glm::mat4& GetModel() const;
	void SetModel(const glm::mat4& model);
	void BatchRender(const std::vector<BatchRenderParams>& params);
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
	void CreateQuad();
};