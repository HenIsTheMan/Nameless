#include "Scene.h"
#include "Vendor/stb_image.h"

extern float angularFOV;
extern float dt;
extern int winWidth;
extern int winHeight;

glm::vec3 Light::globalAmbient = glm::vec3(.2f);

Scene::Scene():
	cam(glm::vec3(0.f, 0.f, 5.f), glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f), 0.f, 150.f),
	mesh(Mesh::MeshType::Quad, GL_TRIANGLES, {
		{"Imgs/BoxAlbedo.png", Mesh::TexType::Diffuse, 0},
		{"Imgs/BoxSpec.png", Mesh::TexType::Spec, 0},
		{"Imgs/BoxEmission.png", Mesh::TexType::Emission, 0},
	}),
	terrain(new Terrain("Imgs/hMap.raw", 8.f, 8.f)),
	model("ObjsAndMtls/nanosuit.obj", {
		aiTextureType_DIFFUSE,
		aiTextureType_SPECULAR,
		//aiTextureType_EMISSIVE,
		//aiTextureType_AMBIENT,
		//aiTextureType_HEIGHT,
	}),
	skydome("ObjsAndMtls/Skydome.obj", {
		aiTextureType_DIFFUSE,
	}),
	blurSP{"Shaders/Quad.vs", "Shaders/Blur.fs"},
	geoPassSP{"Shaders/GeoPass.vs", "Shaders/GeoPass.fs"},
	lightingPassSP{"Shaders/Quad.vs", "Shaders/LightingPass.fs"},
	normalsSP{"Shaders/Normals.vs", "Shaders/Normals.fs", "Shaders/Normals.gs"}, //??
	screenSP{"Shaders/Quad.vs", "Shaders/Screen.fs"},
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
	
	if(terrain){
		delete terrain;
		terrain = nullptr;
	}
	soundEngine->drop();
}

bool Scene::Init(){
	for(int i = 0; i < 5000; ++i){
		params.push_back({
			CreateModelMat(glm::vec3(PseudorandMinMax(-200.f, 200.f), PseudorandMinMax(-200.f, 200.f), -5.f), glm::vec4(0.f, 1.f, 0.f, 0.f), glm::vec3(1.f)),
			glm::vec4(PseudorandMinMax(0.f, 1.f), PseudorandMinMax(0.f, 1.f), PseudorandMinMax(0.f, 1.f), 1.f),
			0,
		});
	};
	for(int i = 0; i < 99999; ++i){
		mesh.AddModelMat(CreateModelMat(glm::vec3(PseudorandMinMax(-2000.f, 2000.f), PseudorandMinMax(-2000.f, 2000.f), -5.f), glm::vec4(0.f, 1.f, 0.f, -45.f), glm::vec3(1.f)));
	}
	for(int i = 0; i < 999; ++i){
		model.AddModelMatForAll(CreateModelMat(glm::vec3(PseudorandMinMax(-100.f, 100.f), PseudorandMinMax(-100.f, 100.f), -5.f), glm::vec4(0.f, 1.f, 0.f, -45.f), glm::vec3(1.f)));
	}
	terrain->AddTexMap({"Imgs/GrassGround.jpg", Mesh::TexType::Diffuse, 0});

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
	static_cast<Spotlight*>(spotlights[0])->diffuse = glm::vec3(100.f, 100.f, 100.f);

	GLint polyMode;
	glGetIntegerv(GL_POLYGON_MODE, &polyMode);
	if(Key(KEY_2) && polyModeBT <= elapsedTime){
		glPolygonMode(GL_FRONT_AND_BACK, polyMode + (polyMode == GL_FILL ? -2 : 1));
		polyModeBT = elapsedTime + .5f;
	}
}

void Scene::GeoPassRender(){
	geoPassSP.Use();

	glDepthFunc(GL_LEQUAL); //Modify comparison operators used for depth test such that frags with depth <= 1.f are shown
	geoPassSP.Set1i("sky", 1);

	geoPassSP.SetMat4fv("PV", &(projection * glm::mat4(glm::mat3(view)))[0][0]);
	skydome.SetModelForAll(CreateModelMat(glm::vec3(0.f, -1.f, 0.f), glm::vec4(0.f, 1.f, 0.f, 0.f), glm::vec3(1.f)));
	skydome.Render(geoPassSP);
	skydome.SetModelForAll(CreateModelMat(glm::vec3(0.f, 1.f, 0.f), glm::vec4(0.f, 0.f, 1.f, 180.f), glm::vec3(1.f)));
	skydome.Render(geoPassSP);

	geoPassSP.Set1i("sky", 0);
	glDepthFunc(GL_LESS);

	geoPassSP.SetMat4fv("PV", &(projection * view)[0][0]);
	terrain->SetModel(CreateModelMat(glm::vec3(0.f), glm::vec4(0.f, 1.f, 0.f, 45.f), glm::vec3(500.f, 100.f, 500.f)));
	terrain->Render(geoPassSP);
	mesh.SetModel(CreateModelMat(glm::vec3(0.f, 1020.f, 0.f), glm::vec4(0.f, 1.f, 0.f, 45.f), glm::vec3(1.f)));
	mesh.InstancedRender(geoPassSP);
	//model.InstancedRender(geoPassSP);

	//mesh.BatchRender(params);
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

	mesh.SetModel(CreateModelMat(glm::vec3(0.f), glm::vec4(0.f, 1.f, 0.f, 0.f), glm::vec3(1.f)));
	mesh.Render(lightingPassSP, false);
	lightingPassSP.ResetTexUnits();
}

void Scene::BlurRender(const uint& brightTexRefID, const bool& horizontal){
	blurSP.Use();
	blurSP.Set1i("horizontal", horizontal);
	blurSP.UseTex(brightTexRefID, "texSampler");
	mesh.SetModel(CreateModelMat(glm::vec3(0.f), glm::vec4(0.f, 1.f, 0.f, 0.f), glm::vec3(1.f)));
	mesh.Render(blurSP, false);
	blurSP.ResetTexUnits();
}

void Scene::DefaultRender(const uint& screenTexRefID, const uint& blurTexRefID){
	screenSP.Use();
	screenSP.UseTex(screenTexRefID, "screenTexSampler");
	screenSP.UseTex(blurTexRefID, "blurTexSampler");
	mesh.SetModel(CreateModelMat(glm::vec3(0.f), glm::vec4(0.f, 1.f, 0.f, 0.f), glm::vec3(1.f)));
	mesh.Render(screenSP, false);
	screenSP.ResetTexUnits();
}