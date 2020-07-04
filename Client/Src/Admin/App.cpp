#include "App.h"

extern float dt;

GLFWwindow* App::win = nullptr;

App::App():
	lastFrameTime(0.f)
{
	if(!InitAPI(win)){
		printf("Failed to init API\n");
	}
}

App::~App(){
	glfwTerminate(); //Clean/Del all GLFW's resources that were allocated
}

void App::Update(){
	float currFrameTime = (float)glfwGetTime();
	dt = currFrameTime - lastFrameTime;
	lastFrameTime = currFrameTime;

	scene.Update();
}

void App::PreRender() const{
	glClearColor(0.2f, 0.3f, 0.3f, 1.f); //State-setting function
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); //State-using function
}

void App::Render(){
	scene.Render();
}

void App::PostRender() const{
	glfwSwapBuffers(win); //Swap the large 2D colour buffer containing colour values for each pixel in GLFW's window
	glfwPollEvents(); //Check for triggered events and call corresponding functions registered via callback methods
}