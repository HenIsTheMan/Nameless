#include "App.h"

extern float dt;

App::App():
	scene(new Scene())
{
	scene->InCtor();
}

App::~App(){
	scene->InDtor();
	delete scene;
}

void App::Init(){
	scene->Init();
}

void App::Update(){
	//scene->FixedUpdate(dt);
	scene->Update(dt);
	scene->LateUpdate(dt);
}

void App::Render(){
	scene->PreRender();
	scene->Render();
	scene->PostRender();
}

Scene* const& App::RetrieveScene(){
	return scene;
}