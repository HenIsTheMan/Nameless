#include "../../Core.h"

template <class T>
void (T::*AppConstruct::sm_InCtor<T>)() = nullptr;

template <class T>
void (T::*AppConstruct::sm_InDtor<T>)() = nullptr;

template <class T>
void (T::*AppConstruct::sm_Init<T>)() = nullptr;

template <class T>
void (T::*AppConstruct::sm_Update<T>)(const float dt) = nullptr;

template <class T>
void (T::*AppConstruct::sm_Render<T>)() = nullptr;

float AppConstruct::lastFrameTime = 0.0f;

template <class T>
void AppConstruct::SetInCtor<T>(void (T::*inCtor)()){
	sm_InCtor = inCtor;
}

template <class T>
void AppConstruct::SetInDtor<T>(void (T::*inDtor)()){
	sm_InDtor = inDtor;
}

template <class T>
void AppConstruct::SetInit<T>(void (T::*init)()){
	sm_Init = init;
}

template <class T>
void AppConstruct::SetUpdate<T>(void (T::*update)(const float dt)){
	sm_Update = update;
}

template <class T>
void AppConstruct::SetRender<T>(void (T::*render)()){
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