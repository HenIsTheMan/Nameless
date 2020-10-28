#include "Scene.h"

Scene::Scene():
	im_Init(nullptr),
	im_FixedUpdate(nullptr),
	im_Update(nullptr),
	im_LateUpdate(nullptr),
	im_PreRender(nullptr),
	im_Render(nullptr),
	im_PostRender(nullptr)
{
}

void Scene::SetInit(void (*init)()){
	im_Init = init;
}

void Scene::SetFixedUpdate(void (*fixedUpdate)(float dt)){
	im_FixedUpdate = fixedUpdate;
}

void Scene::SetUpdate(void (*update)(float dt)){
	im_Update = update;
}

void Scene::SetLateUpdate(void (*lateUpdate)(float dt)){
	im_LateUpdate = lateUpdate;
}

void Scene::SetPreRender(void (*preRender)()){
	im_PreRender = preRender;
}

void Scene::SetRender(void (*render)()){
	im_Render = render;
}

void Scene::SetPostRender(void (*postRender)()){
	im_PostRender = postRender;
}

void Scene::Init(){
	if(im_Init){
		im_Init();
	}
}

void Scene::FixedUpdate(float dt){
	if(im_FixedUpdate){
		im_FixedUpdate(dt);
	}
}

void Scene::Update(float dt){
	if(im_Update){
		im_Update(dt);
	}
}

void Scene::LateUpdate(float dt){
	if(im_LateUpdate){
		im_LateUpdate(dt);
	}
}

void Scene::PreRender(){
	if(im_PreRender){
		im_PreRender();
	}
}

void Scene::Render(){
	if(im_Render){
		im_Render();
	}
}

void Scene::PostRender(){
	if(im_PostRender){
		im_PostRender();
	}
}