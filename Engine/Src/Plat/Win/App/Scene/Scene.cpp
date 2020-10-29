#include "Scene.h"

void (*Scene::im_InCtor)() = nullptr;
void (*Scene::im_InDtor)() = nullptr;
void (*Scene::im_Init)() = nullptr;
void (*Scene::im_FixedUpdate)(float dt) = nullptr;
void (*Scene::im_Update)(float dt) = nullptr;
void (*Scene::im_LateUpdate)(float dt) = nullptr;
void (*Scene::im_PreRender)() = nullptr;
void (*Scene::im_Render)() = nullptr;
void (*Scene::im_PostRender)() = nullptr;

inline void Scene::SetInCtor(void (*inCtor)()){
	im_InCtor = inCtor;
}

inline void Scene::SetInDtor(void (*inDtor)()){
	im_InDtor = inDtor;
}

inline void Scene::SetInit(void (*init)()){
	im_Init = init;
}

inline void Scene::SetFixedUpdate(void (*fixedUpdate)(float dt)){
	im_FixedUpdate = fixedUpdate;
}

inline void Scene::SetUpdate(void (*update)(float dt)){
	im_Update = update;
}

inline void Scene::SetLateUpdate(void (*lateUpdate)(float dt)){
	im_LateUpdate = lateUpdate;
}

inline void Scene::SetPreRender(void (*preRender)()){
	im_PreRender = preRender;
}

inline void Scene::SetRender(void (*render)()){
	im_Render = render;
}

inline void Scene::SetPostRender(void (*postRender)()){
	im_PostRender = postRender;
}

inline void Scene::InCtor(){
	if(im_InCtor){
		im_InCtor();
	}
}

inline void Scene::InDtor(){
	if(im_InDtor){
		im_InDtor();
	}
}

inline void Scene::Init(){
	if(im_Init){
		im_Init();
	}
}

inline void Scene::FixedUpdate(float dt){
	if(im_FixedUpdate){
		im_FixedUpdate(dt);
	}
}

inline void Scene::Update(float dt){
	if(im_Update){
		im_Update(dt);
	}
}

inline void Scene::LateUpdate(float dt){
	if(im_LateUpdate){
		im_LateUpdate(dt);
	}
}

inline void Scene::PreRender(){
	if(im_PreRender){
		im_PreRender();
	}
}

inline void Scene::Render(){
	if(im_Render){
		im_Render();
	}
}

inline void Scene::PostRender(){
	if(im_PostRender){
		im_PostRender();
	}
}