#include "Scene.h"
#define STB_IMAGE_IMPLEMENTATION
#include "Vendor/stb_image.h"

extern float angularFOV;
extern int winWidth;
extern int winHeight;

Scene::Scene():
	cam(glm::vec3(0.f, 0.f, 2.f), glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f), float(winWidth) / float(winHeight), 150.f),
	mesh(Mesh::MeshType::Quad, GL_TRIANGLES),
	shaderProg(ShaderProg("Shaders/Basic.vs", "Shaders/Basic.fs")),
	texRefIDs{}
{
	soundEngine = createIrrKlangDevice(ESOD_AUTO_DETECT, ESEO_MULTI_THREADED | ESEO_LOAD_PLUGINS | ESEO_USE_3D_BUFFERS | ESEO_PRINT_DEBUG_INFO_TO_DEBUGGER);
	//soundEngine->play2D("Audio/Music/YellowCafe.mp3", true);
	//ISound* music = soundEngine->play3D("Audio/Music/YellowCafe.mp3", vec3df(0,0,0), true, false, true);
	//if(music){
	//	music->setMinDistance(5.f);
	//}
	//soundEngine->setListenerPosition(vec3df(0, 0, 0), vec3df(0, 0, 1));
	//if(music){
	//	music->setPosition(vec3df(0, 0, 0));
	//}
}

Scene::~Scene(){
	soundEngine->drop();
	glDeleteTextures(32, texRefIDs);
}

void Scene::Init(){
	cstr imgPaths[]{
		"Imgs/BrickWallAlbedo.jpg",
		"Imgs/Grass.png",
		"Imgs/Water.jpg",
	};
	for(size_t i = 0; i < sizeof(imgPaths) / sizeof(imgPaths[0]); ++i){
		SetUpTex({
			imgPaths[i],
			true,
			GL_TEXTURE_2D,
			GL_REPEAT,
			GL_NEAREST,
			GL_LINEAR,
		}, shaderProg, (const uint)i);
	}
}

void Scene::Update(){
	cam.SetAspectRatio(float(winWidth) / float(winHeight));
	cam.Update(GLFW_KEY_Q, GLFW_KEY_E, GLFW_KEY_A, GLFW_KEY_D, GLFW_KEY_W, GLFW_KEY_S);
	mesh.Update(glm::scale(glm::mat4(1.f), glm::vec3(2.f)), cam.LookAt(), glm::perspective(glm::radians(angularFOV), cam.GetAspectRatio(), .1f, 100.f));
}

void Scene::PreRender() const{
	//Stencil buffer usually contains 8 bits per stencil value that amts to 256 diff stencil values per pixel
	//Use stencil buffer operations to write to the stencil buffer when rendering frags (read stencil values in the same or following frame(s) to pass or discard frags based on their stencil value)
	glEnable(GL_STENCIL_TEST); //Discard frags based on frags of other drawn objs in the scene
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE); //Frags update stencil buffer with their ref value when... //++params and options??

	glEnable(GL_DEPTH_TEST); //Done in screen space after the frag shader has run and after the stencil test //(pass ? frag is rendered and depth buffer is updated with new depth value : frag is discarded)

	glEnable(GL_BLEND); //Colour resulting from blend eqn replaces prev colour stored in the colour buffer
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //++options??
	//glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO); //sets the RGB components as we've set them previously, but only lets the resulting alpha component be influenced by the source's alpha value??
	//glBlendEquation(GL_FUNC_ADD); //Change operator between src and dst part of blend eqn //++other blend eqns??

	glEnable(GL_CULL_FACE); //Specify vertices in CCW winding order so front faces are rendered in CW order while... //Actual winding order is calculated at the rasterization stage after the vertex shader has run //Vertices are then seen from the viewer's POV

	glEnable(GL_PROGRAM_POINT_SIZE);
	//glEnable(GL_MULTISAMPLE); //Enabled by default on most OpenGL drivers //Multisample buffer (stores a given amt of sample pts per pixel) for MSAA
	//Multisampling algs are implemented in the rasterizer (combination of all algs and processes that transform vertices of primitives into frags, frags are bound by screen resolution unlike vertices so there is almost nvr a 1-on-1 mapping between... and it must decide at what screen coords will each frag of each interpolated vertex end up at) of the OpenGL drivers
	//Poor screen resolution leads to aliasing as the limited amt of screen pixels causes some pixels to not be rendered along an edge of a fig //Colour output is stored directly in the framebuffer if pixel is fully covered and blending is disabled

	//glEnable(GL_FRAMEBUFFER_SRGB); //Colours from sRGB colour space are gamma corrected after each frag shader run before they are stored in colour buffers of all framebuffers
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

void Scene::Render(){
	//std::vector<Mesh::BatchRenderParams> params;
	//for(short i = 0; i < 2000; ++i){
	//	params.push_back({
	//		CreateModelMat(glm::vec3(PseudorandMinMax(-100.f, 100.f), PseudorandMinMax(-100.f, 100.f), PseudorandMinMax(-100.f, 100.f)), glm::vec4(0.f, 1.f, 0.f, 0.f), glm::vec3(1.f)),
	//		glm::vec4(PseudorandMinMax(0.f, 1.f), PseudorandMinMax(0.f, 1.f), PseudorandMinMax(0.f, 1.f), 1.f),
	//		PseudorandMinMax(0, 2),
	//	});
	//};
	//mesh.BatchRender(shaderProg, params);
	mesh.Render(shaderProg);
}

void Scene::PostRender() const{
}

void Scene::SetUpTex(const SetUpTexsParams& params, ShaderProg& shaderProg, const uint& texUnit){
	if(texUnit < 0 || texUnit > 31){
		printf("Invalid texUnit!\n");
		return;
	}
	stbi_set_flip_vertically_on_load(params.flipTex); //OpenGL reads y/v tex coord in reverse so must flip tex vertically
	glActiveTexture(GL_TEXTURE0 + texUnit);
	glGenTextures(1, &texRefIDs[texUnit]);
	glBindTexture(params.texTarget, texRefIDs[texUnit]); //Make tex referenced by 'texRefIDs[i]' the tex currently bound to the currently active tex unit so subsequent tex commands will config it
	int width, height, colourChannelsAmt;
	unsigned char* data = stbi_load(params.texPath, &width, &height, &colourChannelsAmt, 0);
	if(data){
		GLenum format1 = colourChannelsAmt == 3 ? GL_RGB16F : GL_RGBA16F;
		GLenum format2 = colourChannelsAmt == 3 ? GL_RGB : GL_RGBA;
		glTexImage2D(params.texTarget, 0, format1, width, height, 0, format2, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(params.texTarget); //Gen required mipmap lvls for currently bound tex
		stbi_image_free(data); //Free the img mem
	} else{
		printf("Failed to load tex at \"%s\"\n", params.texPath);
	}
	glTexParameteri(params.texTarget, GL_TEXTURE_WRAP_S, params.texWrapParam);
	glTexParameteri(params.texTarget, GL_TEXTURE_WRAP_T, params.texWrapParam);
	glTexParameteri(params.texTarget, GL_TEXTURE_MIN_FILTER, params.texFilterMin); //Nearest neighbour/Point filtering/interpolation when textures are scaled downwards
	glTexParameteri(params.texTarget, GL_TEXTURE_MAG_FILTER, params.texFilterMag); //Linear filtering/interpolation for upscaled textures
	shaderProg.Use();
	shaderProg.Set1i(("texSamplers[" + std::to_string(texUnit) + ']').c_str(), texUnit);
}