#include "Scene.h"
#include "Vendor/stb_image.h"

extern float angularFOV;
extern float dt;
extern int winWidth;
extern int winHeight;

glm::vec3 Light::globalAmbient = glm::vec3(.1f);

Scene::Scene():
	cam(glm::vec3(0.f, 0.f, 2.f), glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f), 0.f, 150.f),
	mesh(Mesh::MeshType::Quad, GL_TRIANGLES),
	geoPassSP{"Shaders/GeoPass.vs", "Shaders/GeoPass.fs"},
	lightingPassSP{"Shaders/Quad.vs", "Shaders/LightingPass.fs"},
	screenSP{"Shaders/Quad.vs", "Shaders/Screen.fs"},
	texRefIDs{},
	view(glm::mat4(1.f)),
	projection(glm::mat4(1.f)),
	elapsedTime(0.f),
	polyModeBT(0.f)
{
	soundEngine = createIrrKlangDevice(ESOD_AUTO_DETECT, ESEO_MULTI_THREADED | ESEO_LOAD_PLUGINS | ESEO_USE_3D_BUFFERS | ESEO_PRINT_DEBUG_INFO_TO_DEBUGGER);
	//soundEngine->play2D("Audio/Music/YellowCafe.mp3", true);
	ISound* music = soundEngine->play3D("Audio/Music/YellowCafe.mp3", vec3df(0.f, 0.f, 0.f), true, false, true);
	if(music){
		//music->setPosition(vec3df(0.f, 0.f, 0.f));
		music->setMinDistance(5.f);
		music->setVolume(0);
	}

	mesh.SetModel(CreateModelMat(glm::vec3(0.f), glm::vec4(0.f, 1.f, 0.f, 0.f), glm::vec3(1.f)));
	spotlights.emplace_back(CreateLight(LightType::Spot));
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
		}, geoPassSP, (const uint)i);
	}

	return true;
}

void Scene::Update(){
	elapsedTime += dt;
	cam.SetDefaultAspectRatio(float(winWidth) / float(winHeight));
	cam.ResetAspectRatio();
	cam.Update(GLFW_KEY_Q, GLFW_KEY_E, GLFW_KEY_A, GLFW_KEY_D, GLFW_KEY_W, GLFW_KEY_S);
	const glm::vec3& camPos = cam.GetPos();
	const glm::vec3& camFront = cam.CalcFront();
	soundEngine->setListenerPosition(vec3df(camPos.x, camPos.y, camPos.z), vec3df(camFront.x, camFront.y, camFront.z));
	view = cam.LookAt();
	projection = glm::perspective(glm::radians(angularFOV), cam.GetAspectRatio(), .1f, 9999.f);

	lightingPassSP.Use();
	const int& pAmt = (int)ptLights.size();
	const int& dAmt = (int)directionalLights.size();
	const int& sAmt = (int)spotlights.size();
	lightingPassSP.Set3fv("globalAmbient", Light::globalAmbient);
	for(int i = 0; i < pAmt; ++i){
		lightingPassSP.Set1i("pAmt", pAmt);
		const PtLight* const& ptLight = static_cast<PtLight*>(ptLights[i]);
		lightingPassSP.Set3fv(("ptLights[" + std::to_string(i) + "].ambient").c_str(), ptLight->ambient);
		lightingPassSP.Set3fv(("ptLights[" + std::to_string(i) + "].diffuse").c_str(), ptLight->diffuse);
		lightingPassSP.Set3fv(("ptLights[" + std::to_string(i) + "].specular").c_str(), ptLight->specular);
		lightingPassSP.Set3fv(("ptLights[" + std::to_string(i) + "].pos").c_str(), ptLight->pos);
		lightingPassSP.Set1f(("ptLights[" + std::to_string(i) + "].constant").c_str(), ptLight->constant);
		lightingPassSP.Set1f(("ptLights[" + std::to_string(i) + "].linear").c_str(), ptLight->linear);
		lightingPassSP.Set1f(("ptLights[" + std::to_string(i) + "].quadratic").c_str(), ptLight->quadratic);
	}
	for(int i = 0; i < dAmt; ++i){
		lightingPassSP.Set1i("dAmt", dAmt);
		const DirectionalLight* const& directionalLight = static_cast<DirectionalLight*>(ptLights[i]);
		lightingPassSP.Set3fv(("directionalLights[" + std::to_string(i) + "].ambient").c_str(), directionalLight->ambient);
		lightingPassSP.Set3fv(("directionalLights[" + std::to_string(i) + "].diffuse").c_str(), directionalLight->diffuse);
		lightingPassSP.Set3fv(("directionalLights[" + std::to_string(i) + "].specular").c_str(), directionalLight->specular);
		lightingPassSP.Set3fv(("directionalLights[" + std::to_string(i) + "].dir").c_str(), directionalLight->dir);
	}
	for(int i = 0; i < sAmt; ++i){
		lightingPassSP.Set1i("sAmt", sAmt);
		const Spotlight* const& spotlight = static_cast<Spotlight*>(spotlights[i]);
		lightingPassSP.Set3fv(("spotights[" + std::to_string(i) + "].ambient").c_str(), spotlight->ambient);
		lightingPassSP.Set3fv(("spotights[" + std::to_string(i) + "].diffuse").c_str(), spotlight->diffuse);
		lightingPassSP.Set3fv(("spotights[" + std::to_string(i) + "].specular").c_str(), spotlight->specular);
		lightingPassSP.Set3fv(("spotlights[" + std::to_string(i) + "].pos").c_str(), spotlight->pos);
		lightingPassSP.Set3fv(("spotlights[" + std::to_string(i) + "].dir").c_str(), spotlight->dir);
		lightingPassSP.Set1f(("spotlights[" + std::to_string(i) + "].cosInnerCutoff").c_str(), spotlight->cosInnerCutoff);
		lightingPassSP.Set1f(("spotlights[" + std::to_string(i) + "].cosOuterCutoff").c_str(), spotlight->cosOuterCutoff);
	}
	lightingPassSP.Set1f("mtl.shininess", 32.f); //More light scattering if lower

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
	geoPassSP.Use();
	geoPassSP.SetMat4fv("model", &(mesh.GetModel())[0][0], false);
	glm::mat4 PV = projection * view;
	geoPassSP.SetMat4fv("PV", &(PV)[0][0], false);
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
	screenSP.Use();
	glActiveTexture(GL_TEXTURE31);
	glBindTexture(GL_TEXTURE_2D, texRefID);
	screenSP.Set1i("texSampler", 31);
	screenSP.SetMat4fv("model", &(mesh.GetModel())[0][0], false);
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