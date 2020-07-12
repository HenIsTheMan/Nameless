#pragma once
#include "Vertex.h"
#include "../Graphics/ShaderProg.h"

class Mesh{ //Single drawable geo
	friend class Model;
public:
	enum struct MeshType{ //Pt, Line, 2D (circle, ring and 2D polygons with >= 3 vertices), 3D (sphere, torus and 3D polyhedrons with >= 3 flat polygonal faces)
		None = 0,
		Quad,
		Cuboid,
		Amt
	};
	enum struct TexType{
		Diffuse = 0,
		Spec,
		Emission,
		Reflection,
		Bump,
		Amt
	};
	struct BatchRenderParams final{
		glm::mat4 model;
		glm::vec4 colour;
		int diffuseTexIndex;
	};

	Mesh();
	Mesh(const MeshType& myType, const int& myPrimitive, const std::initializer_list<std::tuple<str, TexType, uint>>& iL);

	///Rule of 5 (prevents shallow copy)
	Mesh(const Mesh& mesh);
	Mesh(Mesh&& mesh) noexcept;
	Mesh& operator=(const Mesh& mesh);
	Mesh& operator=(Mesh&& mesh) noexcept;
	virtual ~Mesh();

	void AddTexMap(const std::tuple<str, TexType, uint>& texMap);
	void RemoveTexMap(str const& texPath);
	void BatchRender(const std::vector<BatchRenderParams>& params);
	void Render(ShaderProg& SP, const glm::mat4& PV, const bool& useTexMaps = true);
	void SetModel(const glm::mat4& model);
protected:
	MeshType type;
	int primitive;
	std::vector<Vertex>* vertices;
	std::vector<uint>* indices;
	std::vector<std::tuple<str, TexType, uint>> texMaps;

	uint batchVAO;
	uint batchVBO;
	uint batchEBO;
	uint VAO;
	uint VBO;
	uint EBO;

	glm::mat4 model;
	void CreateQuad();
};