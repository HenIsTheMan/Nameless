#pragma once

#include "Scene/Scene.h"

class App final{
public:
	App();
	~App();

	void Init();
	void Update();
	void Render();

	Scene* const& RetrieveScene();
private:
	Scene* scene;
};