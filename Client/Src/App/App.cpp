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

	mesh = Mesh(Mesh::MeshType::Quad, GL_TRIANGLES);

	return true;
}

glm::mat4 LookAt(){ //Translate meshes in the scene
	glm::vec3 vecArr[]{glm::vec3(1.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f), -glm::vec3(0.f, 0.f, -1.f)};
	glm::vec3 pos(0.f);

	glm::mat4 translation = glm::mat4(1.f), rotation = glm::mat4(1.f);
	for(short i = 0; i < 3; ++i){ //Access elements as mat[col][row] due to column-major order
		translation[3][i] = -pos[i];
		for(short j = 0; j < 3; ++j){
			rotation[i][j] = (vecArr[j])[i];
		}
	}
	return rotation * translation;
}

void App::Update(){
	mesh.Update(glm::mat4(1.f), LookAt(), glm::perspective(glm::radians(angularFOV), 800.f / 600.f, .1f, 100.f));
}

void App::PreRender() const{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void App::Render(){
	//mesh.Render();
}

void App::PostRender() const{
	glfwSwapBuffers(win); //Swap the large 2D colour buffer containing colour values for each pixel in GLFW's window
	glfwPollEvents(); //Check for triggered events and call corresponding functions registered via callback methods
}