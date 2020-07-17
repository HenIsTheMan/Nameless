#include "SpriteAni.h"
#include "../Engine.h"

extern float dt;

SpriteAnimation::SpriteAnimation(int row, int col): row(row), col(col), currentTime(0), currentFrame(0), playCount(0), currentAnimation(""){}

SpriteAnimation::~SpriteAnimation(){
	for(auto iter = animationList.begin(); iter != animationList.end(); ++iter){
		if(iter->second){
			delete iter->second;
		}
	}
}

void SpriteAnimation::Update(){
	if(animationList[currentAnimation]->animActive){ //Check if the current animation is active
		currentTime += dt;
		int numFrame = (int)animationList[currentAnimation]->frames.size();
		float frameTime = animationList[currentAnimation]->animTime / numFrame;

		currentFrame = animationList[currentAnimation]->frames[std::min((int)animationList[currentAnimation]->frames.size() - 1, int(currentTime / frameTime))]; //Set curr frame based on curr time
		if(currentTime >= animationList[currentAnimation]->animTime){ //If curr time >= total animated time...
			if(playCount < animationList[currentAnimation]->repeatCount){
				///Increase count and repeat
				++playCount;
				currentTime = 0;
				currentFrame = animationList[currentAnimation]->frames[0];
			} else{ //If repeat count is 0 || play count == repeat count...
				animationList[currentAnimation]->animActive = false;
				animationList[currentAnimation]->ended = true;
			}
			if(animationList[currentAnimation]->repeatCount == -1){ //If ani is infinite...
				currentTime = 0.f;
				currentFrame = animationList[currentAnimation]->frames[0];
				animationList[currentAnimation]->animActive = true;
				animationList[currentAnimation]->ended = false;
			}
		}
	}
}

void SpriteAnimation::AddAnimation(std::string anim_name, int start, int end){
	Animation* anim = new Animation();
	for(int i = start; i < end; ++i){ //Ad in all the frames in the range
		anim->AddFrame(i);
	}
	animationList[anim_name] = anim; //Link anim to animation list
	if(currentAnimation == ""){ //Set the current animation if it does not exist
		currentAnimation = anim_name;
	}
	animationList[anim_name]->animActive = false;
}

void SpriteAnimation::AddSequeneAnimation(std::string anim_name, const ::std::initializer_list<int>& frames){
	Animation* anim = new Animation();
	for(const int& frame: frames){
		anim->AddFrame(frame);
	}
	animationList[anim_name] = anim; //Link anim to animation list
	if(currentAnimation == ""){ //Set the current animation if it does not exist
		currentAnimation = anim_name;
	}
	animationList[anim_name]->animActive = false;
}

void SpriteAnimation::PlayAnimation(std::string anim_name, int repeat, float time){
	if(animationList[anim_name] != nullptr){ //Check if the anim name exist
		currentAnimation = anim_name;
		animationList[anim_name]->Set(repeat, time, true);
	}
}

void SpriteAnimation::Resume(){
	animationList[currentAnimation]->animActive = true;
}

void SpriteAnimation::Pause(){
	animationList[currentAnimation]->animActive = false;
}

void SpriteAnimation::Reset(){
	currentFrame = animationList[currentAnimation]->frames[0];
	playCount = 0;
}

void SpriteAnimation::Render(ShaderProg& SP, const bool& autoConfig){
	if(!vertices){
		Create();
	}

	if(primitive < 0){
		return (void)puts("Invalid primitive!\n");
	}

	SP.Use();
	SP.SetMat4fv("model", &(model)[0][0]);
	if(autoConfig){
		SP.Set1i("instancing", 0);

		SP.Set1i("useDiffuseMap", 0);
		SP.Set1i("useSpecMap", 0);
		SP.Set1i("useEmissionMap", 0);
		//SP.Set1i("useReflectionMap", 0);
		SP.Set1i("useBumpMap", 0);

		short diffuseCount = 0;
		for(std::tuple<str, TexType, uint>& texMap: texMaps){
			if(!std::get<uint>(texMap)){
				SetUpTex({
					std::get<str>(texMap),
					true,
					GL_TEXTURE_2D,
					GL_REPEAT,
					GL_LINEAR_MIPMAP_LINEAR,
					GL_LINEAR,
				}, std::get<uint>(texMap));
			}

			switch(std::get<TexType>(texMap)){
				case TexType::Diffuse:
					SP.Set1i("useDiffuseMap", 1);
					SP.UseTex(std::get<uint>(texMap), ("diffuseMaps[" + std::to_string(diffuseCount++) + ']').c_str());
					break;
				case TexType::Spec:
					SP.Set1i("useSpecMap", 1);
					SP.UseTex(std::get<uint>(texMap), "specMap");
					break;
				case TexType::Emission:
					SP.Set1i("useEmissionMap", 1);
					SP.UseTex(std::get<uint>(texMap), "emissionMap");
					break;
				case TexType::Reflection:
					SP.Set1i("useReflectionMap", 1);
					SP.UseTex(std::get<uint>(texMap), "reflectionMap");
					break;
				case TexType::Bump:
					SP.Set1i("useBumpMap", 1);
					SP.UseTex(std::get<uint>(texMap), "bumpMap");
					break;
			}
		}
	}

	if(!VAO){
		switch(type){
			case MeshType::None:
				break;
			case MeshType::Quad:
				CreateQuad();
				break;
		}
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);
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
		glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, diffuseTexIndex));

		if(indices){
			glGenBuffers(1, &EBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices->size() * sizeof(uint), &(*indices)[0], GL_STATIC_DRAW);
		}
		glBindVertexArray(0);
	}

	glBindVertexArray(VAO);
		glDrawElements(primitive, 6, GL_UNSIGNED_INT, (const void*)(long long(this->currentFrame) * 6 * sizeof(GLuint))); //more??
	glBindVertexArray(0);
	if(autoConfig){
		SP.ResetTexUnits();
	}
}

void SpriteAnimation::Create(){
	vertices = new std::vector<Vertex>();
	indices = new std::vector<uint>();

	float width = 1.f / col;
	float height = 1.f / row;
	int offset = 0;
	short myArr[6]{0, 1, 2, 0, 2, 3};
	std::vector<std::pair<glm::vec3, glm::vec2>> temp;

	for(int i = 0; i < row; ++i){
		for(int j = 0; j < col; ++j){
			float U = j * width;
			float V = 1.f - height - i * height;
			temp.push_back({glm::vec3(-.5f, -.5f, 0.f), glm::vec2(U, V)});
			temp.push_back({glm::vec3(.5f, -.5f, 0.f), glm::vec2(U + width, V)});
			temp.push_back({glm::vec3(.5f, .5f, 0.f), glm::vec2(U + width, V + height)});
			temp.push_back({glm::vec3(-.5f, .5f, 0.f), glm::vec2(U, V + height)});

			for(short k = 0; k < 6; ++k){
				indices->emplace_back(offset + myArr[k]);
			}
			offset += 4;
		}
	}

	size_t size = temp.size();
	for(size_t i = 0; i < size; ++i){
		vertices->push_back({
			temp[i].first,
			glm::vec4(.7f, .4f, .1f, 1.f),
			temp[i].second,
			glm::vec3(0.f, 0.f, 1.f),
			glm::vec3(0.f), //??
			0,
		});
	}
}