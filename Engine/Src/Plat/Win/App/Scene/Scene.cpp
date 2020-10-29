#include "Scene.h"

Scene::Scene():
	im_InCtor(nullptr),
	im_InDtor(nullptr),
	im_Init(nullptr),
	im_FixedUpdate(nullptr),
	im_Update(nullptr),
	im_LateUpdate(nullptr),
	im_PreRender(nullptr),
	im_Render(nullptr),
	im_PostRender(nullptr)
{
}

void Scene::SetInCtor(void (*inCtor)()){
	im_InCtor = inCtor;
}

void Scene::SetInDtor(void (*inDtor)()){
	im_InDtor = inDtor;
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

void Scene::InCtor() const{
	if(im_InCtor){
		im_InCtor();
	}
}

void Scene::InDtor() const{
	if(im_InDtor){
		im_InDtor();
	}
}

void Scene::Init() const{
	if(im_Init){
		im_Init();
	}
}

void Scene::FixedUpdate(float dt) const{
	if(im_FixedUpdate){
		im_FixedUpdate(dt);
	}
}

void Scene::Update(float dt) const{
	if(im_Update){
		im_Update(dt);
	}
}

void Scene::LateUpdate(float dt) const{
	if(im_LateUpdate){
		im_LateUpdate(dt);
	}
}

void Scene::PreRender() const{
	if(im_PreRender){
		im_PreRender();
	}
}

void Scene::Render() const{
	if(im_Render){
		im_Render();
	}
}

void Scene::PostRender() const{
	if(im_PostRender){
		im_PostRender();
	}
}