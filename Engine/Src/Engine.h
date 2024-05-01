#pragma once

#include "Constructs/AppConstruct/AppConstruct.h"

#include "Global/GlobalFuncs.h"

extern bool endLoop;

static void User(const int argc, const char* const* const argv); //Forward declaration

///Hmmmm
static int myArgc;
static const char* const* myArgv;

static void MainProcess(){
	User(myArgc, myArgv);

	AppConstruct::InCtor();

	AppConstruct::Init();

	while(!endLoop){
		AppConstruct::Update();
		AppConstruct::Render();
	}

	AppConstruct::InDtor();
}

int main(const int argc, const char* const* const argv){
	if(!InitConsole()){
		return;
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