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
	if(VAO){
		glDeleteVertexArrays(1, &VAO);
	}
	if(VBO){
		glDeleteBuffers(1, &VBO);
	}
	if(EBO){
		glDeleteBuffers(1, &EBO);
	}
}

void Mesh::SetType(const MeshType& type){
	if(vertices){
		delete vertices;
		vertices = nullptr;
	}
	this->type = type;
}

void Mesh::Update(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection){
	this->model = model;
	this->view = view;
	this->projection = projection;
}

void Mesh::BatchRender(ShaderProg& shaderProg, const std::vector<BatchRenderParams>& paramsVec){
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

	shaderProg.Use();
	shaderProg.SetMat4fv("view", &(view)[0][0]);
	shaderProg.SetMat4fv("projection", &(projection)[0][0]);

	std::vector<Vertex> allVertices(paramsVec.size() * vertices->size());
	for(size_t i = 0; i < paramsVec.size(); ++i){
		for(size_t j = 0; j < vertices->size(); ++j){
			allVertices[i * vertices->size() + j] = (*vertices)[j];
			allVertices[i * vertices->size() + j].pos = glm::vec3(paramsVec[i].modelMat * glm::vec4((*vertices)[j].pos, 1.f));
			allVertices[i * vertices->size() + j].colour = paramsVec[i].colour;
			allVertices[i * vertices->size() + j].texIndex = paramsVec[i].texIndex;
		}
	}
	if(!VAO){
		glGenVertexArrays(1, &VAO);
	}
	glBindVertexArray(VAO);
	if(!VBO){
		glGenBuffers(1, &VBO); //A buffer manages a certain piece of GPU mem
		glBindBuffer(GL_ARRAY_BUFFER, VBO); //Makes VBO the buffer currently bound to the GL_ARRAY_BUFFER target, GL_ARRAY_BUFFER is VBO's type
		glBufferData(GL_ARRAY_BUFFER, paramsVec.size() * vertices->size() * sizeof(Vertex), NULL, GL_DYNAMIC_DRAW); //Can combine vertex attrib data into 1 arr or vec and fill VBO's mem with glBufferData

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, pos));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, colour));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, texCoords));
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, normal));
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, tangent));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, texIndex));
	} else{
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
	}
	glBufferSubData(GL_ARRAY_BUFFER, 0, paramsVec.size() * vertices->size() * sizeof(Vertex), &allVertices[0]);

	if(!EBO && indices){
		glGenBuffers(1, &EBO); //Element index buffer
	}
	if(EBO){
		std::vector<uint> allIndices(paramsVec.size() * indices->size());
		for(size_t i = 0; i < paramsVec.size(); ++i){
			for(size_t j = 0; j < indices->size(); ++j){
				allIndices[i * indices->size() + j] = uint((*indices)[j] + vertices->size() * i);
			}
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, paramsVec.size() * indices->size() * sizeof(uint), &allIndices[0], GL_STATIC_DRAW); //Alloc/Reserve a piece of GPU mem and add data into it
		glDrawElements(primitive, (int)allIndices.size(), GL_UNSIGNED_INT, 0); //Draw/Render call/command
	} else{
		glDrawArrays(primitive, 0, (int)allVertices.size()); //...
	}
	glBindVertexArray(0);
}

void Mesh::Render(ShaderProg& shaderProg){
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

	shaderProg.Use();
	shaderProg.SetMat4fv("view", &(view)[0][0]);
	shaderProg.SetMat4fv("projection", &(projection)[0][0]);

	size_t size1 = vertices->size();
	std::vector<Vertex> allVertices(size1);
	for(size_t i = 0; i < size1; ++i){
		allVertices[i].pos = glm::vec3(model * glm::vec4((*vertices)[i].pos, 1.f));
	}

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
		glBufferSubData(GL_ARRAY_BUFFER, 0, vertices->size() * sizeof(Vertex), &allVertices[0]);

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
	if(!vertices){
		glm::vec3 pos[4]{glm::vec3(-1.f, 1.f, 0.f), glm::vec3(-1.f, -1.f, 0.f), glm::vec3(1.f, -1.f, 0.f), glm::vec3(1.f, 1.f, 0.f)};
		glm::vec2 UVs[4]{glm::vec2(0.f, 1.f), glm::vec2(0.f, 0.f), glm::vec2(1.f, 0.f), glm::vec2(1.f, 1.f)};

		///T and B lie on the same plane as normal map surface and align with tex axes U and V so calc them with vertices (to get edges of...) and texCoords (since in tangent space) of primitives
		glm::vec3 tangent[2];
		for(short i = 0; i < 2; ++i){
			glm::vec3 edges[2]{pos[!i ? 1 : 3] - pos[2 * i], pos[2 * !i] - pos[2 * i]};
			glm::vec2 deltaUVs[2]{UVs[!i ? 1 : 3] - UVs[2 * i], UVs[2 * !i] - UVs[2 * i]};
			const float&& reciprocal = 1.f / (deltaUVs[0].x * deltaUVs[1].y - deltaUVs[1].x * deltaUVs[0].y);

			tangent[i].x = reciprocal * (deltaUVs[1].y * edges[0].x - deltaUVs[0].y * edges[1].x);
			tangent[i].y = reciprocal * (deltaUVs[1].y * edges[0].x - deltaUVs[0].y * edges[1].x);
			tangent[i].z = reciprocal * (deltaUVs[1].y * edges[0].x - deltaUVs[0].y * edges[1].x);
		}

		vertices = new std::vector<Vertex>();
		for(short i = 0; i < 4; ++i){
			vertices->push_back({
				pos[i],
				glm::vec4(1.f),
				UVs[i],
				glm::vec3(0.f, 0.f, 1.f),
				tangent[!(i % 3)],
				-1,
			});
		}
		if(indices){
			delete indices;
			indices = nullptr;
		}
		indices = new std::vector<uint>{0, 1, 2, 0, 2, 3};
	}
}