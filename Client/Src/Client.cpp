#include "Engine.h"

#include "MyApp/MyApp.h"

static MyApp* myAppPtr;

void AppInit(){
	myAppPtr = new MyApp();

	myAppPtr->Init();
}

void AppUpdate(const float dt){
	myAppPtr->Update(dt);
}

void AppRender(){
	myAppPtr->Render();
}

void AppTerminate(){
	if(myAppPtr != nullptr){
		delete myAppPtr;
		myAppPtr = nullptr;
	}
}