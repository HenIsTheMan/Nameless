#include "App.h"

#include "../Core.h"

App::App():
	dt(0.0f),
	lastFrameTime(0.0f)
{
	SceneConstruct::InCtor();
}

App::~App(){
	SceneConstruct::InDtor();
}

void App::Init(){
	SceneConstruct::Init();
}

void App::Update(){
	float currFrameTime = (float)glfwGetTime();
	dt = currFrameTime - lastFrameTime;
	lastFrameTime = currFrameTime;

	//SceneConstruct::FixedUpdate(dt);
	SceneConstruct::Update(dt);
	SceneConstruct::LateUpdate(dt);
}

void App::Render(){
	SceneConstruct::PreRender();
	SceneConstruct::Render();
	SceneConstruct::PostRender();
}