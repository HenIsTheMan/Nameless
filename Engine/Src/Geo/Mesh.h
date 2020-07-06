#pragma once
#include "Vertex.h"

class Mesh{ //Single drawable geo
public:
	struct BatchRenderParams final{
		glm::mat4 model;
		glm::vec4 colour;
		int texIndex;
	};
	enum struct MeshType{
		//Pt
		//Line
		//2D (circle, ring and 2D polygons with >= 3 vertices)
		//3D (sphere, torus and 3D polyhedrons with >= 3 flat polygonal faces)
		None = 0,
		Quad,
		Cuboid,
	};
	Mesh();
	Mesh(const MeshType& myType, const int& myPrimitive);
	virtual ~Mesh();
	void BatchRender(const std::vector<BatchRenderParams>& params);
	void Render();

	///Getters
	const glm::mat4& GetModel() const;
	const glm::mat4& GetView() const;
	const glm::mat4& GetProjection() const;

	///Setters
	void SetModel(const glm::mat4& model);
	void SetView(const glm::mat4& view);
	void SetProjection(const glm::mat4& projection);
	//void SetType(const MeshType& type);
	//void SetPrimitive(const int& primitive);
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