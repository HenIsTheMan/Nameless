#include "Scene.h"
#include "Vendor/stb_image.h"

extern float angularFOV;
extern float dt;
extern int winWidth;
extern int winHeight;

Scene::Scene():
	cam(glm::vec3(0.f, 0.f, 2.f), glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f), 0.f, 150.f),
	mesh(Mesh::MeshType::Quad, GL_TRIANGLES),
	basicShaderProg{"Shaders/Basic.vs", "Shaders/Basic.fs"},
	screenShaderProg{"Shaders/Screen.vs", "Shaders/Screen.fs"},
	texRefIDs{},
	view(glm::mat4(1.f)),
	projection(glm::mat4(1.f)),
	elapsedTime(0.f),
	polyModeBT(0.f)
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

	mesh.SetModel(CreateModelMat(glm::vec3(0.f), glm::vec4(0.f, 1.f, 0.f, 0.f), glm::vec3(1.f)));
}

Scene::~Scene(){
	soundEngine->drop();
	glDeleteTextures(32, texRefIDs);
}

bool Scene::Init(){
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
			GL_LINEAR_MIPMAP_LINEAR,
			GL_LINEAR,
		}, basicShaderProg, (const uint)i);
	}

	return true;
}

void Scene::Update(){
	elapsedTime += dt;
	cam.SetDefaultAspectRatio(float(winWidth) / float(winHeight));
	cam.ResetAspectRatio();
	cam.Update(GLFW_KEY_Q, GLFW_KEY_E, GLFW_KEY_A, GLFW_KEY_D, GLFW_KEY_W, GLFW_KEY_S);
	view = cam.LookAt();
	projection = glm::perspective(glm::radians(angularFOV), cam.GetAspectRatio(), .1f, 9999.f);

	GLint polyMode;
	glGetIntegerv(GL_POLYGON_MODE, &polyMode);
	if(Key(KEY_2) && polyModeBT <= elapsedTime){
		glPolygonMode(GL_FRONT_AND_BACK, polyMode + (polyMode == GL_FILL ? -2 : 1));
		polyModeBT = elapsedTime + .5f;
	}
}

void Scene::PreRender() const{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); //State-using function
}

void Scene::RenderToCreatedFB(){
	basicShaderProg.Use();
	basicShaderProg.SetMat4fv("model", &(mesh.GetModel())[0][0], false);
	glm::mat4 PV = projection * view;
	basicShaderProg.SetMat4fv("PV", &(PV)[0][0], false);
	std::vector<Mesh::BatchRenderParams> params;
	for(short i = 0; i < 1; ++i){
		params.push_back({
			CreateModelMat(glm::vec3(0.f), glm::vec4(0.f, 1.f, 0.f, 0.f), glm::vec3(1.f)),
			glm::vec4(PseudorandMinMax(0.f, 1.f), PseudorandMinMax(0.f, 1.f), PseudorandMinMax(0.f, 1.f), 1.f),
			PseudorandMinMax(1, 2),
			});
	};
	mesh.BatchRender(params);
}

void Scene::RenderToDefaultFB(const uint& texRefID){
	screenShaderProg.Use();
	glActiveTexture(GL_TEXTURE31);
	glBindTexture(GL_TEXTURE_2D, texRefID);
	screenShaderProg.Set1i("texSampler", 31);
	screenShaderProg.SetMat4fv("model", &(mesh.GetModel())[0][0], false);
	mesh.Render();
}

void Scene::PostRender() const{
}

void Scene::SetUpTex(const SetUpTexsParams& params, ShaderProg& shaderProg, const uint& texUnit){
	if(texUnit < 0 || texUnit > 31){
		puts("Invalid texUnit!\n");
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