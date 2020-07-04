#pragma once
#include <Engine.h>
#include "Cam.h"

class Scene final{
	Cam cam;
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