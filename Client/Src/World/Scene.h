#pragma once
#include <Engine.h>
#include "Cam.h"

class Scene final{
public:
	Scene();
	~Scene();
	bool Init();
	void Update();
	void PreRender(const float& R, const float& G, const float& B) const;
	void GeoPassRender();
	void LightingPassRender(const uint& posTexRefID, const uint& normalsTexRefID, const uint& albedoSpecTexRefID);
	void RenderToDefaultFB(const uint& texRefID);
	void PostRender() const;
private:
	Cam cam;
	ISoundEngine* soundEngine;
	Mesh mesh;
	Model model;

	ShaderProg geoPassSP;
	ShaderProg lightingPassSP;
	ShaderProg screenSP;

	std::vector<Light*> ptLights;
	std::vector<Light*> directionalLights;
	std::vector<Light*> spotlights;

	std::vector<uint> texRefIDs;
	glm::mat4 view;
	glm::mat4 projection;

	float elapsedTime;
	float polyModeBT;
};

inline glm::mat4 CreateModelMat(const glm::vec3& translate, const glm::vec4& rotate, const glm::vec3& scale){
	return glm::scale(glm::rotate(glm::translate(glm::mat4(1.f), translate), glm::radians(rotate.w), glm::vec3(rotate)), scale);
}