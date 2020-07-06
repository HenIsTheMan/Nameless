#pragma once
#include <Engine.h>
#include "Cam.h"

class Scene final{
	Cam cam;
	ISoundEngine* soundEngine;
	Mesh mesh;
	ShaderProg shaderProg;
	uint texRefIDs[32];
public:
	struct SetUpTexsParams final{
		cstr texPath;
		bool flipTex;
		int texTarget;
		int texWrapParam;
		int texFilterMin;
		int texFilterMag;
	};
	Scene();
	~Scene();
	void Init();
	void Update();
	void PreRender() const;
	void Render();
	void PostRender() const;
	void SetUpTex(const SetUpTexsParams& params, ShaderProg& shaderProg, const uint& texUnit);
};

inline glm::mat4 CreateModelMat(const glm::vec3& translate, const glm::vec4& rotate, const glm::vec3& scale){
	return glm::scale(glm::rotate(glm::translate(glm::mat4(1.f), translate), glm::radians(rotate.w), glm::vec3(rotate)), scale);
}