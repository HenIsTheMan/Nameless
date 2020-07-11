#include "Mesh.h"
#include "../Global/GlobalFuncs.h"

Mesh::Mesh():
	type(MeshType::None),
	primitive(GL_TRIANGLES),
	vertices(nullptr),
	indices(nullptr),
	texMaps{},
	batchVAO(0),
	batchVBO(0),
	batchEBO(0),
	VAO(0),
	VBO(0),
	EBO(0),
	model(glm::mat4(1.f))
{
}

Mesh::Mesh(const MeshType& myType, const int& myPrimitive, const std::initializer_list<std::tuple<cstr, TexType, uint>>& iL):
	type(myType),
	primitive(myPrimitive),
	vertices(nullptr),
	indices(nullptr),
	texMaps(iL),
	batchVAO(0),
	batchVBO(0),
	batchEBO(0),
	VAO(0),
	VBO(0),
	EBO(0),
	model(glm::mat4(1.f))
{
}

Mesh::Mesh(const Mesh& mesh): Mesh(){
	if(this != &mesh){
		vertices = new std::vector<Vertex>(mesh.vertices->begin(), mesh.vertices->end());
		indices = new std::vector<uint>{mesh.indices->begin(), mesh.indices->end()};
	}
}

Mesh::Mesh(Mesh&& mesh) noexcept: Mesh(){
	if(this != &mesh){
		vertices = new std::vector<Vertex>(mesh.vertices->begin(), mesh.vertices->end());
		indices = new std::vector<uint>{mesh.indices->begin(), mesh.indices->end()};
	}
}

Mesh& Mesh::operator=(const Mesh& mesh){
	if(this != &mesh){
		vertices = new std::vector<Vertex>(mesh.vertices->begin(), mesh.vertices->end());
		indices = new std::vector<uint>{mesh.indices->begin(), mesh.indices->end()};
	}
	return *this;
}

Mesh& Mesh::operator=(Mesh&& mesh) noexcept{
	if(this != &mesh){
		vertices = new std::vector<Vertex>(mesh.vertices->begin(), mesh.vertices->end());
		indices = new std::vector<uint>{mesh.indices->begin(), mesh.indices->end()};
	}
	return *this;
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
	if(batchVAO){
		glDeleteVertexArrays(1, &batchVAO);
	}
	if(batchVBO){
		glDeleteBuffers(1, &batchVBO);
	}
	if(batchEBO){
		glDeleteBuffers(1, &batchEBO);
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

void Mesh::BatchRender(const std::vector<BatchRenderParams>& paramsVec){
	if(primitive < 0){
		puts("Invalid primitive!\n");
		return;
	}
	switch(type){
		case MeshType::None:
			break;
		case MeshType::Quad:
			CreateQuad();
			break;
	}

	const size_t paramsVecSize = paramsVec.size();
	const size_t verticesSize = vertices->size();
	std::vector<Vertex> allVertices(paramsVecSize * verticesSize);
	for(size_t i = 0; i < paramsVecSize; ++i){
		for(size_t j = 0; j < verticesSize; ++j){
			allVertices[i * verticesSize + j] = (*vertices)[j];
			allVertices[i * verticesSize + j].pos = glm::vec3(paramsVec[i].model * glm::vec4((*vertices)[j].pos, 1.f));
			allVertices[i * verticesSize + j].colour = paramsVec[i].colour;
			allVertices[i * verticesSize + j].texIndex = paramsVec[i].texIndex;
		}
	}
	if(!batchVAO){
		glGenVertexArrays(1, &batchVAO);
	}
	glBindVertexArray(batchVAO);
	if(!batchVBO){
		glGenBuffers(1, &batchVBO); //A buffer manages a certain piece of GPU mem
		glBindBuffer(GL_ARRAY_BUFFER, batchVBO); //Makes batchVBO the buffer currently bound to the GL_ARRAY_BUFFER target, GL_ARRAY_BUFFER is batchVBO's type
		glBufferData(GL_ARRAY_BUFFER, paramsVec.size() * vertices->size() * sizeof(Vertex), NULL, GL_DYNAMIC_DRAW); //Can combine vertex attrib data into 1 arr or vec and fill batchVBO's mem with glBufferData

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
		glBindBuffer(GL_ARRAY_BUFFER, batchVBO);
	}
	glBufferSubData(GL_ARRAY_BUFFER, 0, paramsVec.size() * vertices->size() * sizeof(Vertex), &allVertices[0]);

	if(!batchEBO && indices){
		glGenBuffers(1, &batchEBO); //Element index buffer
	}
	if(batchEBO){
		const size_t paramsVecSize = paramsVec.size();
		const size_t indicesSize = indices->size();
		std::vector<uint> allIndices(paramsVecSize * indicesSize);
		for(size_t i = 0; i < paramsVecSize; ++i){
			for(size_t j = 0; j < indicesSize; ++j){
				allIndices[i * indicesSize + j] = uint((*indices)[j] + verticesSize * i);
			}
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, batchEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, paramsVecSize * indicesSize * sizeof(uint), &allIndices[0], GL_STATIC_DRAW); //Alloc/Reserve a piece of GPU mem and add data into it
		glDrawElements(primitive, (int)allIndices.size(), GL_UNSIGNED_INT, 0); //Draw/Render call/command
	} else{
		glDrawArrays(primitive, 0, (int)allVertices.size()); //...
	}
	glBindVertexArray(0);
}

void Mesh::Render(ShaderProg& SP, const glm::mat4& PV){
	if(primitive < 0){
		puts("Invalid primitive!\n");
		return;
	}

	SP.Use();
	SP.SetMat4fv("model", &(model)[0][0]);
	//SP.SetMat4fv("PV", &(PV)[0][0]);

	//const size_t size = texMaps.size();
	//for(size_t i = 0; i < size; ++i){
	//	SetUpTex({
	//		std::get<cstr>(texMaps[i]),
	//		true,
	//		GL_TEXTURE_2D,
	//		GL_REPEAT,
	//		GL_LINEAR_MIPMAP_LINEAR,
	//		GL_LINEAR,
	//	}, std::get<uint>(texMaps[i]));

	//	switch(std::get<TexType>(texMaps[i])){
	//		case TexType::Diffuse:
	//			SP.UseTex(std::get<uint>(texMaps[i]), ("diffuseMaps[" + std::to_string(i) + ']').c_str());
	//			break;
	//		case TexType::Spec:
	//			SP.UseTex(std::get<uint>(texMaps[i]), "specMap");
	//			break;
	//		case TexType::Emission:
	//			SP.UseTex(std::get<uint>(texMaps[i]), "emissionMap");
	//			break;
	//		case TexType::Reflection:
	//			SP.UseTex(std::get<uint>(texMaps[i]), "reflectionMap");
	//			break;
	//		case TexType::Bump:
	//			SP.UseTex(std::get<uint>(texMaps[i]), "bumpMap");
	//			break;
	//	}
	//}

	if(!VAO){
		switch(type){
			case MeshType::None:
				break;
			case MeshType::Quad:
				CreateQuad();
				break;
		}

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
			glGenBuffers(1, &VBO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, vertices->size() * sizeof(Vertex), &(*vertices)[0], GL_STATIC_DRAW);

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

			if(indices){
				glGenBuffers(1, &EBO);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices->size() * sizeof(uint), &(*indices)[0], GL_STATIC_DRAW);
			}
		glBindVertexArray(0);
	}
	
	glBindVertexArray(VAO);
		indices ? glDrawElements(primitive, (int)indices->size(), GL_UNSIGNED_INT, 0) : glDrawArrays(primitive, 0, (int)vertices->size());
	glBindVertexArray(0);
	SP.ResetTexUnits();
}

void Mesh::SetModel(const glm::mat4& model){
	this->model = model;
}

//void Mesh::SetType(const MeshType& type){
//	if(vertices){
//		delete vertices;
//		vertices = nullptr;
//	}
//	this->type = type;
//}
//
//void Mesh::SetPrimitive(const int& primitive){
//	this->primitive = primitive;
//}

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
				glm::vec4(.5f),
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