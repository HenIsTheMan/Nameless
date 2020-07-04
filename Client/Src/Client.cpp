#include "Client.h"
#include "App/Global.h"

bool endLoop = false;

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
	SetConsoleTitleA("Nameless Console");
	srand(uint(glfwGetTime()));
	system("Color 0A");

	HANDLE StdHandle = GetStdHandle(DWORD(-11));
	CONSOLE_CURSOR_INFO cursorInfo;
	GetConsoleCursorInfo(StdHandle, &cursorInfo);
	cursorInfo.bVisible = 0;
	SetConsoleCursorInfo(StdHandle, &cursorInfo);

	::ShowWindow(::GetConsoleWindow(), SW_SHOW);
	if(!SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleEventHandler, TRUE)){
		printf("Failed to install console event handler!\n");
		return -1;
	}

	double list[]{3.4, 2.1, 5.4};
	SortArr(list, 0, 2, SortOrder::Ascending, SortingAlgType::Bubble);
	PrintArrElements(list, 0, 2, PrintFormat::CSListHoriz);

	std::thread worker(&MainProcess);
	while(!endLoop){
		if(Key(VK_ESCAPE)){
			endLoop = true;
			break;
		}
	}
	worker.join();
}