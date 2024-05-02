#pragma once

#include "Global/GlobalFuncs.h"

extern bool endLoop;
extern int myArgc;
extern const char* const* myArgv;

//* Forward declarations
static void AppInit();
static void AppUpdate(const float dt);
static void AppRender();
static void AppTerminate();
//*/

static void MainProcess(){
	AppInit();

	float currFrameTime;
	float dt;
	float lastFrameTime = 0.0f;

	while(!endLoop){
		currFrameTime = (float)glfwGetTime();

		dt = currFrameTime - lastFrameTime;

		lastFrameTime = currFrameTime;

		AppUpdate(dt);
		AppRender();
	}

	AppTerminate();
}

int main(const int argc, const char* const* const argv){
	if(!InitConsole()){
		return -1;
	}

	myArgc = argc;
	myArgv = argv;

	std::thread worker(&MainProcess);

	while(!endLoop){
		if(Key(VK_ESCAPE)){
			endLoop = true;
			break;
		}
	}

	worker.join();
}