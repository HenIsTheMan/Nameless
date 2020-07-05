#pragma once
#include <Engine.h>
#include "Cam.h"

class Scene final{
	Cam cam;
	ISoundEngine* soundEngine;
	Mesh mesh;
	ShaderProg shaderProg;
public:
	Scene();
	~Scene();
	void Update();
	void PreRender() const;
	void Render();
	void PostRender() const;
};

inline glm::mat4 CreateModelMat(const glm::vec3& translate, const glm::vec4& rotate, const glm::vec3& scale){
	return glm::scale(glm::rotate(glm::translate(glm::mat4(1.f), translate), glm::radians(rotate.w), glm::vec3(rotate)), scale);
}