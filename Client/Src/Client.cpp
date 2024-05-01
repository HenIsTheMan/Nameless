#include "App/App.h"

#include "Scenes/YesScene.h"

extern bool endLoop;

void MainProcess(){
	SceneConstruct::SetInCtor(YesScene::InCtor);
	SceneConstruct::SetInDtor(YesScene::InDtor);
	SceneConstruct::SetInit(YesScene::Init);
	SceneConstruct::SetFixedUpdate(YesScene::FixedUpdate);
	SceneConstruct::SetUpdate(YesScene::Update);
	SceneConstruct::SetLateUpdate(YesScene::LateUpdate);
	SceneConstruct::SetPreRender(YesScene::PreRender);
	SceneConstruct::SetRender(YesScene::Render);
	SceneConstruct::SetPostRender(YesScene::PostRender);

	App* app = new App();
	app->Init();

	while(!endLoop){
		app->Update();
		app->Render();
	}

	delete app;
}

int main(const int&, const char* const* const&){
	if(!InitConsole()){
		return -1;
	}

	std::thread worker(&MainProcess);

	while(!endLoop){
		if(Key(VK_ESCAPE)){
			endLoop = true;
			break;
		}
	}

	worker.join();
}