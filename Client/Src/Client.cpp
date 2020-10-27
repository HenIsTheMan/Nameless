#include <Core.h>
#include <Engine.h>

extern bool endLoop;

void MainProcess(){
	App* app = new App();
	while(!endLoop){
		app->Update();
		app->PreRender();
		app->Render();
		app->PostRender();
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