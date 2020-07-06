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
	void Init();
	void Update();
	void PreRender() const;
	void Render();
	void PostRender() const;
};

inline glm::mat4 CreateModelMat(const glm::vec3& translate, const glm::vec4& rotate, const glm::vec3& scale){
	return glm::scale(glm::rotate(glm::translate(glm::mat4(1.f), translate), glm::radians(rotate.w), glm::vec3(rotate)), scale);
}

struct SetUpTexsParams final{
	cstr texPath;
	bool flipTex;
	int texTarget;
	int texWrapParam;
	int texFilterMin;
	int texFilterMag;
};

void SetUpTex(const SetUpTexsParams& params, ShaderProg& shaderProg, const uint& texUnit);