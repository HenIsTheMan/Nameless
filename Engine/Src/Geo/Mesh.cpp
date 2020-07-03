#include "Mesh.h"

Mesh::Mesh():
	type(MeshType::None),
	primitive(-1),
	vertices(nullptr),
	indices(nullptr),
	VAO(0),
	VBO(0),
	EBO(0),
	model(glm::mat4(1.f)),
	view(glm::mat4(1.f)),
	projection(glm::mat4(1.f))
{
}

Mesh::Mesh(const MeshType& myType, const int& myPrimitive):
	type(myType),
	primitive(myPrimitive),
	vertices(nullptr),
	indices(nullptr),
	VAO(0),
	VBO(0),
	EBO(0),
	model(glm::mat4(1.f)),
	view(glm::mat4(1.f)),
	projection(glm::mat4(1.f))
{
}

Mesh::~Mesh(){
	if(vertices){
		delete vertices;
		vertices = nullptr;
	}
	if(indices){
		delete indices;
		indices = nullptr;
	}
}

void Mesh::Update(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection){
	this->model = model;
	this->view = view;
	this->projection = projection;
}

void Mesh::Render(){
	if(primitive < 0){
		return;
	}
	switch(type){
		case MeshType::None:
			return;
		case MeshType::Quad:
			CreateQuad();
			break;
	}
	//??
	indices ? glDrawElements(primitive, (int)indices->size(), GL_UNSIGNED_INT, 0) : glDrawArrays(primitive, 0, (int)vertices->size()); //Draw/Render call/command
}

void Mesh::CreateQuad(){
	glm::vec3 pos[4]{glm::vec3(-1.f, 1.f, 0.f), glm::vec3(-1.f, -1.f, 0.f), glm::vec3(1.f, -1.f, 0.f), glm::vec3(1.f, 1.f, 0.f)};
	glm::vec2 UVs[4]{glm::vec2(0.f, 1.f), glm::vec2(0.f, 0.f), glm::vec2(1.f, 0.f), glm::vec2(1.f, 1.f)};

	///T and B lie on the same plane as normal map surface and align with tex axes U and V so calc them with vertices (to get edges of...) and texCoords (since in tangent space) of primitives
	glm::vec3 tangent[2];
	//glm::vec3 bitangent[2];
	for(short i = 0; i < 2; ++i){
		glm::vec3 edges[2]{pos[!i ? 1 : 3] - pos[2 * i], pos[2 * !i] - pos[2 * i]};
		glm::vec2 deltaUVs[2]{UVs[!i ? 1 : 3] - UVs[2 * i], UVs[2 * !i] - UVs[2 * i]};
		const float&& reciprocal = 1.f / (deltaUVs[0].x * deltaUVs[1].y - deltaUVs[1].x * deltaUVs[0].y);

		tangent[i].x = reciprocal * (deltaUVs[1].y * edges[0].x - deltaUVs[0].y * edges[1].x);
		tangent[i].y = reciprocal * (deltaUVs[1].y * edges[0].x - deltaUVs[0].y * edges[1].x);
		tangent[i].z = reciprocal * (deltaUVs[1].y * edges[0].x - deltaUVs[0].y * edges[1].x);

		//bitangent[i].x = reciprocal * (-deltaUVs[1].x * edges[0].x + deltaUVs[0].x * edges[1].x);
		//bitangent[i].y = reciprocal * (-deltaUVs[1].x * edges[0].y + deltaUVs[0].x * edges[1].y);
		//bitangent[i].z = reciprocal * (-deltaUVs[1].x * edges[0].z + deltaUVs[0].x * edges[1].z);
	}

	if(vertices && vertices->size() != 4){
		delete vertices;
		vertices = nullptr;
	}
	if(!vertices){
		vertices = new std::vector<Vertex>();
		for(short i = 0; i < 4; ++i){
			vertices->emplace_back(Vertex(pos[i], glm::vec4(1.f), UVs[i], std::vector<uint>{}, glm::vec3(0.f, 0.f, 1.f), tangent[!(i % 3)])); //bitangent[i > 1]
		}
	}
	if(indices && indices->size() != 6){
		delete indices;
		indices = nullptr;
	}
	if(!indices){
		indices = new std::vector<uint>{0, 1, 2, 0, 2, 3};
	}
}