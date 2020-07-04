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

	shaderProg = ShaderProg("Shaders/Basic.vs", "Shaders/Basic.fs");
	mesh = Mesh(Mesh::MeshType::Quad, GL_TRIANGLES);

	return true;
}

glm::mat4 LookAt(){ //Translate meshes in the scene
	glm::vec3 vecArr[]{glm::vec3(1.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f), -glm::vec3(0.f, 0.f, -1.f)};
	glm::vec3 pos(0.f, 0.f, 1.f);

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
	mesh.Update(glm::scale(glm::mat4(1.f), glm::vec3(.5f, .5f, 1.f)), LookAt(), glm::perspective(glm::radians(angularFOV), 800.f / 600.f, .1f, 100.f));
}

void App::PreRender() const{
	glClearColor(0.2f, 0.3f, 0.3f, 1.f); //State-setting function
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); //State-using function

	//Stencil buffer usually contains 8 bits per stencil value that amts to 256 diff stencil values per pixel
	//Use stencil buffer operations to write to the stencil buffer when rendering frags (read stencil values in the same or following frame(s) to pass or discard frags based on their stencil value)
	glEnable(GL_STENCIL_TEST); //Discard frags based on frags of other drawn objs in the scene
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE); //Frags update stencil buffer with their ref value when... //++params and options??

	glEnable(GL_DEPTH_TEST); //Done in screen space after the frag shader has run and after the stencil test //(pass ? frag is rendered and depth buffer is updated with new depth value : frag is discarded)

	glEnable(GL_BLEND); //Colour resulting from blend eqn replaces prev colour stored in the colour buffer
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //++options??
	//glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO); //sets the RGB components as we've set them previously, but only lets the resulting alpha component be influenced by the source's alpha value??
	//glBlendEquation(GL_FUNC_ADD); //Change operator between src and dst part of blend eqn //++other blend eqns??

	//glEnable(GL_CULL_FACE); //Specify vertices in CCW winding order so front faces are rendered in CW order while... //Actual winding order is calculated at the rasterization stage after the vertex shader has run //Vertices are then seen from the viewer's POV

	glEnable(GL_PROGRAM_POINT_SIZE);
	//glEnable(GL_MULTISAMPLE); //Enabled by default on most OpenGL drivers //Multisample buffer (stores a given amt of sample pts per pixel) for MSAA
	//Multisampling algs are implemented in the rasterizer (combination of all algs and processes that transform vertices of primitives into frags, frags are bound by screen resolution unlike vertices so there is almost nvr a 1-on-1 mapping between... and it must decide at what screen coords will each frag of each interpolated vertex end up at) of the OpenGL drivers
	//Poor screen resolution leads to aliasing as the limited amt of screen pixels causes some pixels to not be rendered along an edge of a fig //Colour output is stored directly in the framebuffer if pixel is fully covered and blending is disabled

	//glEnable(GL_FRAMEBUFFER_SRGB); //Colours from sRGB colour space are gamma corrected after each frag shader run before they are stored in colour buffers of all framebuffers
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

void App::Render(){
	shaderProg.Use();
	mesh.Render();
}

void App::PostRender() const{
	glfwSwapBuffers(win); //Swap the large 2D colour buffer containing colour values for each pixel in GLFW's window
	glfwPollEvents(); //Check for triggered events and call corresponding functions registered via callback methods
}