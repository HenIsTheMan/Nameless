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
	model("ObjsAndMtls/nanosuit.obj", {}), //aiTextureType_DIFFUSE, aiTextureType_SPECULAR, aiTextureType_EMISSIVE, aiTextureType_HEIGHT, aiTextureType_AMBIENT
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
	const size_t pSize = ptLights.size();
	const size_t dSize = directionalLights.size();
	const size_t sSize = spotlights.size();
	for(size_t i = 0; i < pSize; ++i){
		delete ptLights[i];
		ptLights[i] = nullptr;
	}
	for(size_t i = 0; i < dSize; ++i){
		delete directionalLights[i];
		directionalLights[i] = nullptr;
	}
	for(size_t i = 0; i < sSize; ++i){
		delete spotlights[i];
		spotlights[i] = nullptr;
	}
	soundEngine->drop();
	glDeleteTextures((int)texRefIDs.size(), &texRefIDs[0]);
}

bool Scene::Init(){
	cstr imgPaths[]{
		"Imgs/BoxAlbedo.png",
		"Imgs/BoxSpec.png",
		"Imgs/BoxEmission.png",
	};
	for(short i = 0; i < sizeof(imgPaths) / sizeof(imgPaths[0]); ++i){
		texRefIDs.emplace_back();
		SetUpTex({
			imgPaths[i],
			true,
			GL_TEXTURE_2D,
			GL_REPEAT,
			GL_LINEAR_MIPMAP_LINEAR,
			GL_LINEAR,
		}, texRefIDs[i]);
	}

	return true;
}

void Scene::Update(){
	elapsedTime += dt;
	if(winHeight){ //Avoid division by 0 when win is minimised
		cam.SetDefaultAspectRatio(float(winWidth) / float(winHeight));
		cam.ResetAspectRatio();
	}
	cam.Update(GLFW_KEY_Q, GLFW_KEY_E, GLFW_KEY_A, GLFW_KEY_D, GLFW_KEY_W, GLFW_KEY_S);
	view = cam.LookAt();
	projection = glm::perspective(glm::radians(angularFOV), cam.GetAspectRatio(), .1f, 9999.f);

	const glm::vec3& camPos = cam.GetPos();
	const glm::vec3& camFront = cam.CalcFront();
	soundEngine->setListenerPosition(vec3df(camPos.x, camPos.y, camPos.z), vec3df(camFront.x, camFront.y, camFront.z));
	static_cast<Spotlight*>(spotlights[0])->pos = camPos;
	static_cast<Spotlight*>(spotlights[0])->dir = camFront;
	//static_cast<Spotlight*>(spotlights[0])->diffuse = glm::vec3(100.f, 100.f, 100.f);

	GLint polyMode;
	glGetIntegerv(GL_POLYGON_MODE, &polyMode);
	if(Key(KEY_2) && polyModeBT <= elapsedTime){
		glPolygonMode(GL_FRONT_AND_BACK, polyMode + (polyMode == GL_FILL ? -2 : 1));
		polyModeBT = elapsedTime + .5f;
	}
}

void Scene::GeoPassRender(){
	geoPassSP.Use();
	geoPassSP.UseTex(texRefIDs[0], "diffuseMaps[0]");
	geoPassSP.Set1i("useSpecMap", 1);
	geoPassSP.UseTex(texRefIDs[1], "specMap");
	geoPassSP.Set1i("useEmissionMap", 1);
	geoPassSP.UseTex(texRefIDs[2], "emissionMap");

	geoPassSP.SetMat4fv("model", &(mesh.GetModel())[0][0]);
	glm::mat4 PV = projection * view;
	geoPassSP.SetMat4fv("PV", &(PV)[0][0]);

	std::vector<Mesh::BatchRenderParams> params;
	for(short i = 0; i < 1; ++i){
		params.push_back({
			CreateModelMat(glm::vec3(0.f), glm::vec4(0.f, 1.f, 0.f, 0.f), glm::vec3(1.f)),
			glm::vec4(PseudorandMinMax(0.f, 1.f), PseudorandMinMax(0.f, 1.f), PseudorandMinMax(0.f, 1.f), 1.f),
			0,
			});
	};
	mesh.BatchRender(params);

	//glm::mat4 modelMat = glm::mat4(1.f);
	//geoPassSP.SetMat4fv("model", &modelMat[0][0]);
	//model.Render(GL_TRIANGLES);

	geoPassSP.ResetTexUnits();
}

void Scene::LightingPassRender(const uint& posTexRefID, const uint& normalsTexRefID, const uint& albedoSpecTexRefID){
	lightingPassSP.Use();
	const int& pAmt = (int)ptLights.size();
	const int& dAmt = (int)directionalLights.size();
	const int& sAmt = (int)spotlights.size();
	lightingPassSP.Set1f("mtl.shininess", 32.f); //More light scattering if lower
	lightingPassSP.Set3fv("globalAmbient", Light::globalAmbient);

	lightingPassSP.Set3fv("camPos", cam.GetPos());
	lightingPassSP.UseTex(posTexRefID, "posTex");
	lightingPassSP.UseTex(normalsTexRefID, "normalsTex");
	lightingPassSP.UseTex(albedoSpecTexRefID, "albedoSpecTex");

	for(int i = 0; i < pAmt; ++i){
		lightingPassSP.Set1i("pAmt", pAmt);
		const PtLight* const& ptLight = static_cast<PtLight*>(ptLights[i]);
		lightingPassSP.Set3fv(("ptLights[" + std::to_string(i) + "].ambient").c_str(), ptLight->ambient);
		lightingPassSP.Set3fv(("ptLights[" + std::to_string(i) + "].diffuse").c_str(), ptLight->diffuse);
		lightingPassSP.Set3fv(("ptLights[" + std::to_string(i) + "].spec").c_str(), ptLight->spec);
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
		lightingPassSP.Set3fv(("directionalLights[" + std::to_string(i) + "].spec").c_str(), directionalLight->spec);
		lightingPassSP.Set3fv(("directionalLights[" + std::to_string(i) + "].dir").c_str(), directionalLight->dir);
	}
	for(int i = 0; i < sAmt; ++i){
		lightingPassSP.Set1i("sAmt", sAmt);
		const Spotlight* const& spotlight = static_cast<Spotlight*>(spotlights[i]);
		lightingPassSP.Set3fv(("spotlights[" + std::to_string(i) + "].ambient").c_str(), spotlight->ambient);
		lightingPassSP.Set3fv(("spotlights[" + std::to_string(i) + "].diffuse").c_str(), spotlight->diffuse);
		lightingPassSP.Set3fv(("spotlights[" + std::to_string(i) + "].spec").c_str(), spotlight->spec);
		lightingPassSP.Set3fv(("spotlights[" + std::to_string(i) + "].pos").c_str(), spotlight->pos);
		lightingPassSP.Set3fv(("spotlights[" + std::to_string(i) + "].dir").c_str(), spotlight->dir);
		lightingPassSP.Set1f(("spotlights[" + std::to_string(i) + "].cosInnerCutoff").c_str(), spotlight->cosInnerCutoff);
		lightingPassSP.Set1f(("spotlights[" + std::to_string(i) + "].cosOuterCutoff").c_str(), spotlight->cosOuterCutoff);
	}

	lightingPassSP.SetMat4fv("model", &(mesh.GetModel())[0][0]);
	mesh.Render();
	lightingPassSP.ResetTexUnits();
}

void Scene::RenderToDefaultFB(const uint& texRefID){
	screenSP.Use();
	screenSP.UseTex(texRefID, "texSampler");
	screenSP.SetMat4fv("model", &(mesh.GetModel())[0][0]);
	mesh.Render();
	screenSP.ResetTexUnits();
}