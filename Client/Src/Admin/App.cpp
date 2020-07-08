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
	FBORefIDs{},
	texRefIDs{},
	RBORefIDs{}
{
	if(!InitAPI(win)){
		puts("Failed to init API\n");
		endLoop = true;
	}
	(void)Init();
}

App::~App(){
	glDeleteTextures(sizeof(texRefIDs) / sizeof(texRefIDs[0]), texRefIDs);
	glDeleteRenderbuffers(sizeof(RBORefIDs) / sizeof(RBORefIDs[0]), RBORefIDs);
	glDeleteFramebuffers(sizeof(FBORefIDs) / sizeof(FBORefIDs[0]), FBORefIDs);
	glfwTerminate(); //Clean/Del all GLFW's resources that were allocated
}

bool App::Init(){
	glGenFramebuffers(sizeof(FBORefIDs) / sizeof(FBORefIDs[0]), FBORefIDs);
	glGenTextures(sizeof(texRefIDs) / sizeof(texRefIDs[0]), texRefIDs);
	glGenRenderbuffers(sizeof(RBORefIDs) / sizeof(RBORefIDs[0]), RBORefIDs);
	stbi_set_flip_vertically_on_load(true);

	glBindFramebuffer(GL_FRAMEBUFFER, FBORefIDs[(int)FBO::GeoPass]);
		for(Tex i = Tex::Pos; i <= Tex::AlbedoSpec; ++i){
			glBindTexture(GL_TEXTURE_2D, texRefIDs[(int)i]);
				glTexImage2D(GL_TEXTURE_2D, 0, i == Tex::AlbedoSpec ? GL_RGBA : GL_RGBA16F, winWidth, winHeight, 0, GL_RGBA, i == Tex::AlbedoSpec ? GL_UNSIGNED_BYTE : GL_FLOAT, NULL);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + (int)i, GL_TEXTURE_2D, texRefIDs[(int)i], 0);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		uint GeoCA[]{GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
		glDrawBuffers(3, GeoCA);

		glBindRenderbuffer(GL_RENDERBUFFER, RBORefIDs[0]);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, winWidth, winHeight);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBORefIDs[0]);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
			printf(STR(FBO::GeoPass));
			puts(" is incomplete!\n");
			return false;
		}
	glBindFramebuffer(GL_FRAMEBUFFER, FBORefIDs[(int)FBO::LightingPass]);
		for(Tex i = Tex::Lit; i <= Tex::BrightLit; ++i){
			glBindTexture(GL_TEXTURE_2D, texRefIDs[(int)i]);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, winWidth, winHeight, 0, GL_RGBA, GL_FLOAT, NULL);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + int(i) - int(Tex::Lit), GL_TEXTURE_2D, texRefIDs[(int)i], 0);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		uint LightingCA[]{GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
		glDrawBuffers(2, LightingCA);

		if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
			printf(STR(FBO::LightingPass));
			puts(" is incomplete!\n");
			return false;
		}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	(void)InitOptions();
	(void)scene.Init();
	glPointSize(10.f);
	glLineWidth(5.f);

	return true;
}

bool App::InitOptions() const{
	//Stencil buffer usually contains 8 bits per stencil value that amts to 256 diff stencil values per pixel
	//Use stencil buffer operations to write to the stencil buffer when rendering frags (read stencil values in the same or following frame(s) to pass or discard frags based on their stencil value)
	glEnable(GL_STENCIL_TEST); //Discard frags based on frags of other drawn objs in the scene
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE); //Frags update stencil buffer with their ref value when... //++params and options??

	glEnable(GL_DEPTH_TEST); //Done in screen space after the frag shader has run and after the stencil test //(pass ? frag is rendered and depth buffer is updated with new depth value : frag is discarded)

	glEnable(GL_BLEND); //Colour resulting from blend eqn replaces prev colour stored in the colour buffer
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //++options??
	//glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO); //sets the RGB components as we've set them previously, but only lets the resulting alpha component be influenced by the source's alpha value??
	//glBlendEquation(GL_FUNC_ADD); //Change operator between src and dst part of blend eqn //++other blend eqns??

	glEnable(GL_CULL_FACE); //Specify vertices in CCW winding order so front faces are rendered in CW order while... //Actual winding order is calculated at the rasterization stage after the vertex shader has run //Vertices are then seen from the viewer's POV

	glEnable(GL_PROGRAM_POINT_SIZE);
	//glEnable(GL_MULTISAMPLE); //Enabled by default on most OpenGL drivers //Multisample buffer (stores a given amt of sample pts per pixel) for MSAA
	//Multisampling algs are implemented in the rasterizer (combination of all algs and processes that transform vertices of primitives into frags, frags are bound by screen resolution unlike vertices so there is almost nvr a 1-on-1 mapping between... and it must decide at what screen coords will each frag of each interpolated vertex end up at) of the OpenGL drivers
	//Poor screen resolution leads to aliasing as the limited amt of screen pixels causes some pixels to not be rendered along an edge of a fig //Colour output is stored directly in the framebuffer if pixel is fully covered and blending is disabled

	//glEnable(GL_FRAMEBUFFER_SRGB); //Colours from sRGB colour space are gamma corrected after each frag shader run before they are stored in colour buffers of all framebuffers
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	return true;
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
	glBindFramebuffer(GL_FRAMEBUFFER, FBORefIDs[(int)FBO::GeoPass]);
	scene.PreRender(1.f, 0.82f, 0.86f);
	scene.GeoPassRender();
	scene.PostRender();

	glBindFramebuffer(GL_FRAMEBUFFER, FBORefIDs[(int)FBO::LightingPass]);
	scene.PreRender(1.f, 0.82f, 0.86f);
	scene.LightingPassRender(texRefIDs[(int)Tex::Pos], texRefIDs[(int)Tex::Normals], texRefIDs[(int)Tex::AlbedoSpec]);
	scene.PostRender();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	scene.PreRender(1.f, 0.82f, 0.86f);
	scene.RenderToDefaultFB(texRefIDs[(int)Tex::Lit]);
	scene.PostRender();
}

void App::PostRender() const{
	glfwSwapBuffers(win); //Swap the large 2D colour buffer containing colour values for each pixel in GLFW's window
	glfwPollEvents(); //Check for triggered events and call corresponding functions registered via callback methods
}