#pragma once

#include "Graphics/Cam.h"

#include "Geo/Model.h"
#include "Geo/ModelStack.h"

#include "Graphics/Light.h"
#include "Graphics/ShaderProg.h"
#include "Graphics/TextChief.h"

class MyScene final{
public:
	MyScene();
	~MyScene();

	bool Init();
	void Update(const float dt);
	void GeoRenderPass();
	void LightingRenderPass(
		const uint& posTexRefID,
		const uint& coloursTexRefID,
		const uint& normalsTexRefID,
		const uint& specTexRefID,
		const uint& reflectionTexRefID
	);
	void BlurRender(const uint& brightTexRefID, const bool& horizontal);
	void DefaultRender(const uint& screenTexRefID, const uint& blurTexRefID);
	void ForwardRender();
private:
	Cam cam;
	ISoundEngine* soundEngine;
	ISound* music;
	ISoundEffectControl* soundFX;
	TextChief textChief;

	enum struct MeshType{
		Quad = 0,
		Cube,
		Sphere,
		Cylinder,
		SpriteAni,
		Terrain,
		Amt
	};
	Mesh* meshes[(int)MeshType::Amt];

	enum struct ModelType{
		Skydome = 0,
		Suit,
		Amt
	};
	Model* models[(int)ModelType::Amt];

	ShaderProg blurSP;
	ShaderProg lightingPassSP;
	ShaderProg normalsSP;
	ShaderProg screenSP;
	ShaderProg textSP;


	ShaderProg* forwardSP;
	ShaderProg* geoPassSP;


	std::vector<Light*> ptLights;
	std::vector<Light*> directionalLights;
	std::vector<Light*> spotlights;

	glm::mat4 view;
	glm::mat4 projection;
	//std::vector<Mesh::BatchRenderParams> params;

	float FPS;
	float elapsedTime;
	int polyMode;
	ModelStack modelStack;
};