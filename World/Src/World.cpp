#include "World.h"

bool endLoop = false;

static inline const bool Key(const char& key){
	return GetAsyncKeyState((unsigned short)key) & 0x8000;
}

void MainProcess(){
	App* app = App::GetObjPtr();
	if(!app->Init()){
		endLoop = true;
	}
	while(!endLoop){
		app->Update();
		app->PreRender();
		app->Render();
		app->PostRender();
	}
	app->Destroy();
}

BOOL ConsoleEventHandler(const DWORD event){
	LPCWSTR msg;
	switch(event){
		case CTRL_C_EVENT: msg = L"Ctrl + C"; break;
		case CTRL_BREAK_EVENT: msg = L"Ctrl + BREAK"; break;
		case CTRL_CLOSE_EVENT: msg = L"Closing prog..."; break;
		case CTRL_LOGOFF_EVENT: case CTRL_SHUTDOWN_EVENT: msg = L"User is logging off..."; break;
		default: msg = L"???";
	}
	MessageBox(NULL, msg, L"Nameless", MB_OK);
	return TRUE;
}

int main(){
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	system("Color 0A");
	srand(uint(glfwGetTime()));
	::ShowWindow(::GetConsoleWindow(), SW_SHOW);
	if(!SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleEventHandler, TRUE)){
		printf("Failed to install console event handler!\n");
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