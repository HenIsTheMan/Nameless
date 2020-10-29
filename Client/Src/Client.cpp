#include "Plat/Win/App/App.h"

#include "Scenes/YesScene.h"

extern bool endLoop;

void MainProcess(){
	App* app = new App();
	app->Init();
	Scene* const& scene = app->RetrieveScene();

	scene->SetInCtor(YesScene::InCtor);
	scene->SetInDtor(YesScene::InDtor);
	scene->SetInit(YesScene::Init);
	scene->SetFixedUpdate(YesScene::FixedUpdate);
	scene->SetUpdate(YesScene::Update);
	scene->SetLateUpdate(YesScene::LateUpdate);
	scene->SetPreRender(YesScene::PreRender);
	scene->SetRender(YesScene::Render);
	scene->SetPostRender(YesScene::PostRender);

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