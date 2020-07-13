#pragma once
#include <Engine.h>
#include "Cam.h"

class Scene final{
public:
	Scene();
	~Scene();
	bool Init();
	void Update();
	void GeoPassRender();
	void LightingPassRender(const uint& posTexRefID, const uint& normalsTexRefID, const uint& albedoSpecTexRefID);
	void BlurRender(const uint& brightTexRefID, const bool& horizontal);
	void DefaultRender(const uint& screenTexRefID, const uint& blurTexRefID);
private:
	Cam cam;
	ISoundEngine* soundEngine;
	Mesh mesh;
	Model model;

	ShaderProg blurSP;
	ShaderProg geoPassSP;
	ShaderProg lightingPassSP;
	ShaderProg normalsSP;
	ShaderProg screenSP;

	std::vector<Light*> ptLights;
	std::vector<Light*> directionalLights;
	std::vector<Light*> spotlights;

	glm::mat4 view;
	glm::mat4 projection;
	std::vector<Mesh::BatchRenderParams> params;

	float elapsedTime;
	float polyModeBT;
};

inline glm::mat4 CreateModelMat(const glm::vec3& translate, const glm::vec4& rotate, const glm::vec3& scale){
	return glm::scale(glm::rotate(glm::translate(glm::mat4(1.f), translate), glm::radians(rotate.w), glm::vec3(rotate)), scale);
}