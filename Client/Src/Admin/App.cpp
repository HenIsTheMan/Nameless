#include "App.h"
#define STB_IMAGE_IMPLEMENTATION
#include "Vendor/stb_image.h"

extern float dt;
extern bool endLoop;
extern int winWidth;
extern int winHeight;

GLFWwindow* App::win = nullptr;

App::App():
	lastFrameTime(0.f),
	scene(),
	gBufferRefID(0),
	texRefIDs{},
	RBORefID(0)
{
	if(!InitAPI(win)){
		puts("Failed to init API\n");
		endLoop = true;
	}

	glGenFramebuffers(1, &gBufferRefID);
	glBindFramebuffer(GL_FRAMEBUFFER, gBufferRefID);
		size_t size = sizeof(texRefIDs) / sizeof(texRefIDs[0]);
		stbi_set_flip_vertically_on_load(true);
		glGenTextures((int)size, texRefIDs);
		for(size_t i = 0; i < size; ++i){
			glBindTexture(GL_TEXTURE_2D, texRefIDs[i]);
				glTexImage2D(GL_TEXTURE_2D, 0, i == size - 1 ? GL_RGBA : GL_RGBA16F, winWidth, winHeight, 0, GL_RGBA, i == size - 1 ? GL_UNSIGNED_BYTE : GL_FLOAT, NULL);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + (int)i, GL_TEXTURE_2D, texRefIDs[i], 0);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		unsigned int colAttachments[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
		glDrawBuffers(3, colAttachments);

		glGenRenderbuffers(1, &RBORefID);
		glBindRenderbuffer(GL_RENDERBUFFER, RBORefID);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, winWidth, winHeight);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBORefID);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
			puts("Created framebuffer is incomplete.\n");
		}
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

	scene.Init();
	glPointSize(50.f);
	glLineWidth(2.f);
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
}

void App::Render(){
	scene.Render(gBufferRefID);

	glActiveTexture(GL_TEXTURE31);
	glBindTexture(GL_TEXTURE_2D, texRefIDs[2]);
	scene.Render(0);
}

void App::PostRender() const{
	glfwSwapBuffers(win); //Swap the large 2D colour buffer containing colour values for each pixel in GLFW's window
	glfwPollEvents(); //Check for triggered events and call corresponding functions registered via callback methods
}