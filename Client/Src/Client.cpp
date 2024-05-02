#include "Engine.h"

#include "Constructs/SceneConstruct/SceneConstruct.h"

#include "MyApp/MyApp.h"
#include "Scenes/MyScene/MyScene.h"

void User(const int argc, const char* const* const argv){
	MyApp* myApp = new MyApp();
	MyScene* myScene = new MyScene();

	AppConstruct::SetInCtor(MyApp::InCtor);
	AppConstruct::SetInDtor(MyApp::InDtor);
	AppConstruct::SetInit(MyApp::Init);
	AppConstruct::SetUpdate(MyApp::Update);
	AppConstruct::SetRender(MyApp::Render);

	//SceneConstruct::SetInCtor(myScene->InCtor);
	//SceneConstruct::SetInDtor(void (*inDtor)());
	//SceneConstruct::SetInit(void (*init)());
	//SceneConstruct::SetFixedUpdate(void (*fixedUpdate)(const float dt));
	//SceneConstruct::SetUpdate(void (*update)(const float dt));
	//SceneConstruct::SetLateUpdate(void (*lateUpdate)(const float dt));
	//SceneConstruct::SetPreRender(void (*preRender)());
	//SceneConstruct::SetRender(void (*render)());
	//SceneConstruct::SetPostRender(void (*postRender)());

	if(myApp != nullptr){
		delete myApp;
		myApp = nullptr;
	}

	if(myScene != nullptr){
		delete myScene;
		myScene = nullptr;
	}
}