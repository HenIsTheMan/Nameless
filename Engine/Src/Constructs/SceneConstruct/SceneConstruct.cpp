#include "SceneConstruct.h"

void (*SceneConstruct::sm_InCtor)() = nullptr;
void (*SceneConstruct::sm_InDtor)() = nullptr;
void (*SceneConstruct::sm_Init)() = nullptr;
void (*SceneConstruct::sm_FixedUpdate)(const float dt) = nullptr;
void (*SceneConstruct::sm_Update)(const float dt) = nullptr;
void (*SceneConstruct::sm_LateUpdate)(const float dt) = nullptr;
void (*SceneConstruct::sm_PreRender)() = nullptr;
void (*SceneConstruct::sm_Render)() = nullptr;
void (*SceneConstruct::sm_PostRender)() = nullptr;

void SceneConstruct::SetInCtor(void (*inCtor)()){
	sm_InCtor = inCtor;
}

void SceneConstruct::SetInDtor(void (*inDtor)()){
	sm_InDtor = inDtor;
}

void SceneConstruct::SetInit(void (*init)()){
	sm_Init = init;
}

void SceneConstruct::SetFixedUpdate(void (*fixedUpdate)(const float dt)){
	sm_FixedUpdate = fixedUpdate;
}

void SceneConstruct::SetUpdate(void (*update)(const float dt)){
	sm_Update = update;
}

void SceneConstruct::SetLateUpdate(void (*lateUpdate)(const float dt)){
	sm_LateUpdate = lateUpdate;
}

void SceneConstruct::SetPreRender(void (*preRender)()){
	sm_PreRender = preRender;
}

void SceneConstruct::SetRender(void (*render)()){
	sm_Render = render;
}

void SceneConstruct::SetPostRender(void (*postRender)()){
	sm_PostRender = postRender;
}

void SceneConstruct::InCtor(){
	if(sm_InCtor){
		sm_InCtor();
	}
}

void SceneConstruct::InDtor(){
	if(sm_InDtor){
		sm_InDtor();
	}
}

void SceneConstruct::Init(){
	if(sm_Init){
		sm_Init();
	}
}

void SceneConstruct::FixedUpdate(const float dt){
	if(sm_FixedUpdate){
		sm_FixedUpdate(dt);
	}
}

void SceneConstruct::Update(const float dt){
	if(sm_Update){
		sm_Update(dt);
	}
}

void SceneConstruct::LateUpdate(const float dt){
	if(sm_LateUpdate){
		sm_LateUpdate(dt);
	}
}

void SceneConstruct::PreRender(){
	if(sm_PreRender){
		sm_PreRender();
	}
}

void SceneConstruct::Render(){
	if(sm_Render){
		sm_Render();
	}
}

void SceneConstruct::PostRender(){
	if(sm_PostRender){
		sm_PostRender();
	}
}