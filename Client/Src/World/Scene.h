#pragma once
#include <Engine.h>
#include "Cam.h"

class Scene final{
	Cam cam;
	ISoundEngine* soundEngine = createIrrKlangDevice();
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