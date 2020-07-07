#pragma once
#include <Engine.h>
#include "Cam.h"

class Scene final{
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
	bool Init();
	void Update();
	void PreRender() const;
	void RenderToCreatedFB();
	void RenderToDefaultFB(const uint& texRefID);
	void PostRender() const;
private:
	Cam cam;
	ISoundEngine* soundEngine;
	Mesh mesh;
	ShaderProg basicShaderProg;
	ShaderProg screenShaderProg;
	uint texRefIDs[32];
	glm::mat4 view;
	glm::mat4 projection;
	void SetUpTex(const SetUpTexsParams& params, ShaderProg& shaderProg, const uint& texUnit);

	float elapsedTime;
	float polyModeBT;
};

inline glm::mat4 CreateModelMat(const glm::vec3& translate, const glm::vec4& rotate, const glm::vec3& scale){
	return glm::scale(glm::rotate(glm::translate(glm::mat4(1.f), translate), glm::radians(rotate.w), glm::vec3(rotate)), scale);
}