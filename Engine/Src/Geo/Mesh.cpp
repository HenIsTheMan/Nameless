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

	//float vertices[] = {
	//	-1.f, 1.f, 0.f,
	//	-1.f, -1.f, 0.f,
	//	1.f, -1.f, 0.f,
	//	1.f, 1.f, 0.f,
	//};

	//unsigned int indices[] = {
	//	0, 1, 2, 0, 2, 3,
	//};

	//glGenVertexArrays(1, &VAO);
	//glBindVertexArray(VAO);

	//glGenBuffers(1, &VBO); //Gen VBO and get ref ID of it
	//glBindBuffer(GL_ARRAY_BUFFER, VBO); //Makes VBO the buffer currently bound to the GL_ARRAY_BUFFER target, GL_ARRAY_BUFFER is VBO's type
	//glBufferData(GL_ARRAY_BUFFER, vertices->size() * sizeof(Vertex), &(*vertices)[0], GL_STATIC_DRAW); //Copies vertex data stored in 'vertices' into VBO's mem //diff types??

	//glGenBuffers(1, &EBO);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); //GL_ELEMENT_ARRAY_BUFFER is the buffer target
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices->size() * sizeof(uint), &(*indices)[0], GL_STATIC_DRAW);

	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, pos));

	//glDrawElements(primitive, (int)indices->size(), GL_UNSIGNED_INT, 0);
	//glBindVertexArray(0); //
	
	if(!VAO){
		glGenVertexArrays(1, &VAO);
	}
	glBindVertexArray(VAO);
		if(!VBO){
			glGenBuffers(1, &VBO); //A buffer manages a certain piece of GPU mem
			glBindBuffer(GL_ARRAY_BUFFER, VBO); //Makes VBO the buffer currently bound to the GL_ARRAY_BUFFER target, GL_ARRAY_BUFFER is VBO's type
			glBufferData(GL_ARRAY_BUFFER, vertices->size() * sizeof(Vertex), NULL, GL_DYNAMIC_DRAW); //Can combine vertex attrib data into 1 arr or vec and fill VBO's mem with glBufferData

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, pos));
		} else{
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
		}
		glBufferSubData(GL_ARRAY_BUFFER, 0, vertices->size() * sizeof(Vertex), &(*vertices)[0]);

		if(!EBO && indices){
			glGenBuffers(1, &EBO); //Element index buffer
		}
		if(EBO){
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices->size() * sizeof(uint), &(*indices)[0], GL_STATIC_DRAW); //Alloc/Reserve a piece of GPU mem and add data into it
			glDrawElements(primitive, (int)indices->size(), GL_UNSIGNED_INT, 0);
		} else{
			glDrawArrays(primitive, 0, (int)vertices->size()); //Draw/Render call/command
		}
	glBindVertexArray(0);
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

	for(short i = 0; i < 4; ++i){
		pos[i] = glm::vec3(projection * view * model * glm::vec4(pos[i], 1.f));
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