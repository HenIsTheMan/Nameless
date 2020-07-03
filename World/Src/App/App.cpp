#include "App.h"
#include <App/API.h>

GLFWwindow* App::win = nullptr;

App::~App(){
	glfwTerminate(); //Clean/Del all GLFW's resources that were allocated
}

bool App::Init(){
	if(!InitAPI(win)){
		return false;
	}
	return true;
}

void App::Update(){
}

void App::PreRender() const{
}

void App::Render() const{
}

void App::PostRender() const{
	glfwSwapBuffers(win); //Swap the large 2D colour buffer containing colour values for each pixel in GLFW's window
	glfwPollEvents(); //Check for triggered events and call corresponding functions registered via callback methods
}