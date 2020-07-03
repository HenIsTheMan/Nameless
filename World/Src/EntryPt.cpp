#include <Engine.h>

static bool endLoop = false;

static inline const bool Key(const char& key){
	return GetAsyncKeyState((unsigned short)key) & 0x8000;
}

void MainLoop(){
	//App* app = new App; //Implement Factory Method design pattern??
	//Cam* cam = new Cam(glm::vec3(-20.f, 0.f, 130.f), glm::vec3(-20.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
	//while(!endLoop){ //Main loop
	//	app->Update(*cam);
	//	cam->Update(GLFW_KEY_Q, GLFW_KEY_E, GLFW_KEY_A, GLFW_KEY_D, GLFW_KEY_W, GLFW_KEY_S);
	//	app->Render(*cam);
	//	glfwSwapBuffers(App::win); //Swap the large 2D colour buffer containing colour values for each pixel in GLFW's window
	//	glfwPollEvents(); //Check for triggered events and call corresponding functions registered via callback methods
	//}
	//delete app;
	//delete cam;
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

	std::thread mainLooper(MainLoop);
	while(!endLoop){
		if(Key(VK_ESCAPE)){
			endLoop = true;
			break;
		}
	}
	mainLooper.join();
}