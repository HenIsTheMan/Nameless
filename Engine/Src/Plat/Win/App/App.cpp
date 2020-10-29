#include "App.h"

extern float dt;

App::App(){
	Scene::InCtor();
}

App::~App(){
	Scene::InDtor();
}

void App::Init(){
	Scene::Init();
}

void App::Update(){
	//Scene::FixedUpdate(dt);
	Scene::Update(dt);
	Scene::LateUpdate(dt);
}

void App::Render(){
	Scene::PreRender();
	Scene::Render();
	Scene::PostRender();
}