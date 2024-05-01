#include "AppConstruct.h"

#include "../../Core.h"

void (*AppConstruct::sm_InCtor)() = nullptr;
void (*AppConstruct::sm_InDtor)() = nullptr;
void (*AppConstruct::sm_Init)() = nullptr;
void (*AppConstruct::sm_Update)(const float dt) = nullptr;
void (*AppConstruct::sm_Render)() = nullptr;

float AppConstruct::lastFrameTime = 0.0f;

void AppConstruct::SetInCtor(void (*inCtor)()){
	sm_InCtor = inCtor;
}

void AppConstruct::SetInDtor(void (*inDtor)()){
	sm_InDtor = inDtor;
}

void AppConstruct::SetInit(void (*init)()){
	sm_Init = init;
}

void AppConstruct::SetUpdate(void (*update)(const float dt)){
	sm_Update = update;
}

void AppConstruct::SetRender(void (*render)()){
	sm_Render = render;
}

void AppConstruct::InCtor(){
	if(sm_InCtor){
		sm_InCtor();
	}
}

void AppConstruct::InDtor(){
	if(sm_InDtor){
		sm_InDtor();
	}
}

void AppConstruct::Init(){
	if(sm_Init){
		sm_Init();
	}
}

void AppConstruct::Update(){
	const float currFrameTime = (float)glfwGetTime();

	const float dt = currFrameTime - lastFrameTime;

	lastFrameTime = currFrameTime;

	if(sm_Update){
		sm_Update(dt);
	}
}

void AppConstruct::Render(){
	if(sm_Render){
		sm_Render();
	}
}