#include "Scene.h"
#include "Vendor/stb_image.h"

extern float angularFOV;
extern float dt;
extern int winWidth;
extern int winHeight;

glm::vec3 Light::globalAmbient = glm::vec3(.2f);

Scene::Scene():
	cam(glm::vec3(0.f, 0.f, 5.f), glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f), 0.f, 150.f),
	soundEngine(nullptr),
	music(nullptr),
	soundFX(nullptr),
	meshes{
		new Mesh(Mesh::MeshType::Quad, GL_TRIANGLES, {
			{"Imgs/BoxAlbedo.png", Mesh::TexType::Diffuse, 0},
			{"Imgs/BoxSpec.png", Mesh::TexType::Spec, 0},
			{"Imgs/BoxEmission.png", Mesh::TexType::Emission, 0},
		}),
		new Mesh(Mesh::MeshType::Cube, GL_TRIANGLES, {
		}),
		new Mesh(Mesh::MeshType::Sphere, GL_TRIANGLE_STRIP, {
		}),
		new Mesh(Mesh::MeshType::Cylinder, GL_TRIANGLE_STRIP, {
			{"Imgs/BoxAlbedo.png", Mesh::TexType::Diffuse, 0},
		}),
		new SpriteAni(4, 8),
		new Terrain("Imgs/hMap.raw", 8.f, 8.f),
	},
	model("ObjsAndMtls/nanosuit.obj", {
		aiTextureType_DIFFUSE,
		aiTextureType_SPECULAR,
		aiTextureType_EMISSIVE,
		aiTextureType_AMBIENT,
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
	textSP{"Shaders/Text.vs", "Shaders/Text.fs"},
	view(glm::mat4(1.f)),
	projection(glm::mat4(1.f)),
	elapsedTime(0.f),
	polyModeBT(0.f),
	distortionBT(0.f),
	echoBT(0.f),
	wavesReverbBT(0.f),
	resetSoundFXBT(0.f),
	modelStack()
{
	soundEngine = createIrrKlangDevice(ESOD_AUTO_DETECT, ESEO_MULTI_THREADED | ESEO_LOAD_PLUGINS | ESEO_USE_3D_BUFFERS | ESEO_PRINT_DEBUG_INFO_TO_DEBUGGER);
	if(!soundEngine){
		(void)puts("Failed to init soundEngine!\n");
	}
	//soundEngine->play2D("Audio/Music/YellowCafe.mp3", true);

	music = soundEngine->play3D("Audio/Music/YellowCafe.mp3", vec3df(0.f, 0.f, 0.f), true, false, true, ESM_AUTO_DETECT, true);
	if(music){
		//music->setPosition(vec3df(0.f, 0.f, 0.f));
		music->setMinDistance(5.f);
		music->setVolume(0);

		soundFX = music->getSoundEffectControl();
		if(!soundFX){
			(void)puts("No soundFX support!\n");
		}
	} else{
		(void)puts("Failed to init music!\n");
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
	
	for(int i = 0; i < (int)GeoType::Amt; ++i){
		if(meshes[i]){
			delete meshes[i];
			meshes[i] = nullptr;
		}
	}
	if(music){
		music->drop();
	}
	if(soundEngine){
		soundEngine->drop();
	}
}

bool Scene::Init(){
	for(int i = 0; i < 5000; ++i){
		PushModel({
			Translate(glm::vec3(PseudorandMinMax(-200.f, 200.f), PseudorandMinMax(-200.f, 200.f), -5.f)),
		});
			params.push_back({
				GetTopModel(),
				glm::vec4(PseudorandMinMax(0.f, 1.f), PseudorandMinMax(0.f, 1.f), PseudorandMinMax(0.f, 1.f), 1.f),
				0,
			});
		PopModel();
	};
	for(int i = 0; i < 99999; ++i){
		PushModel({
			Translate(glm::vec3(PseudorandMinMax(-2000.f, 2000.f), PseudorandMinMax(-2000.f, 2000.f), -5.f)),
			Rotate(glm::vec4(0.f, 1.f, 0.f, -45.f)),
		});
			meshes[(int)GeoType::Quad]->AddModelMat(GetTopModel());
		PopModel();
	}
	for(int i = 0; i < 999; ++i){
		PushModel({
			Translate(glm::vec3(PseudorandMinMax(-100.f, 100.f), PseudorandMinMax(-100.f, 100.f), -5.f)),
			Rotate(glm::vec4(0.f, 1.f, 0.f, -45.f)),
		});
			model.AddModelMatForAll(GetTopModel());
		PopModel();
	}

	meshes[(int)GeoType::SpriteAni]->AddTexMap({"Imgs/Fire.png", Mesh::TexType::Diffuse, 0});
	static_cast<SpriteAni*>(meshes[(int)GeoType::SpriteAni])->AddAni("FireSpriteAni", 0, 32);
	static_cast<SpriteAni*>(meshes[(int)GeoType::SpriteAni])->Play("FireSpriteAni", -1, .5f);

	meshes[(int)GeoType::Terrain]->AddTexMap({"Imgs/GrassGround.jpg", Mesh::TexType::Diffuse, 0});

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
	//static_cast<Spotlight*>(spotlights[0])->diffuse = glm::vec3(50.f); //Blocky light if too high??
	//static_cast<Spotlight*>(spotlights[0])->spec = glm::vec3(0.f, 1.f, 1.f); //Coloured specular highlight
	static_cast<SpriteAni*>(meshes[(int)GeoType::SpriteAni])->Update();

	GLint polyMode;
	glGetIntegerv(GL_POLYGON_MODE, &polyMode);
	if(Key(KEY_2) && polyModeBT <= elapsedTime){
		glPolygonMode(GL_FRONT_AND_BACK, polyMode + (polyMode == GL_FILL ? -2 : 1));
		polyModeBT = elapsedTime + .5f;
	}

	if(soundFX){
		if(Key(KEY_I) && distortionBT <= elapsedTime){
			soundFX->isDistortionSoundEffectEnabled() ? soundFX->disableDistortionSoundEffect() : (void)soundFX->enableDistortionSoundEffect();
			distortionBT = elapsedTime + .5f;
		}
		if(Key(KEY_O) && echoBT <= elapsedTime){
			soundFX->isEchoSoundEffectEnabled() ? soundFX->disableEchoSoundEffect() : (void)soundFX->enableEchoSoundEffect();
			echoBT = elapsedTime + .5f;
		}
		if(Key(KEY_P) && wavesReverbBT <= elapsedTime){
			soundFX->isWavesReverbSoundEffectEnabled() ? soundFX->disableWavesReverbSoundEffect() : (void)soundFX->enableWavesReverbSoundEffect();
			wavesReverbBT = elapsedTime + .5f;
		}
		if(Key(KEY_L) && resetSoundFXBT <= elapsedTime){
			soundFX->disableAllEffects();
			resetSoundFXBT = elapsedTime + .5f;
		}
	}
}

void Scene::GeoRenderPass(){
	geoPassSP.Use();
	//geoPassSP.Set3fv("fog.colour", glm::vec3(.7f));
	//geoPassSP.Set1f("fog.start", 100.f);
	//geoPassSP.Set1f("fog.end", 800.f);
	//geoPassSP.Set1f("fog.density", .01f);
	//geoPassSP.Set1i("fog.type", 2);

	glDepthFunc(GL_LEQUAL); //Modify comparison operators used for depth test such that frags with depth <= 1.f are shown
	geoPassSP.Set1i("sky", 1);
	geoPassSP.SetMat4fv("PV", &(projection * glm::mat4(glm::mat3(view)))[0][0]);
	PushModel({
		Translate(glm::vec3(0.f, -1.f, 0.f)),
	});
		skydome.SetModelForAll(GetTopModel());
		skydome.Render(geoPassSP);
	PopModel();
	PushModel({
		Translate(glm::vec3(0.f, 1.f, 0.f)),
		Rotate(glm::vec4(0.f, 0.f, 1.f, 180.f)),
	});
		skydome.SetModelForAll(GetTopModel());
		skydome.Render(geoPassSP);
	PopModel();
	geoPassSP.Set1i("sky", 0);
	glDepthFunc(GL_LESS);

	geoPassSP.SetMat4fv("PV", &(projection * view)[0][0]);

	///Terrain
	PushModel({
		Rotate(glm::vec4(0.f, 1.f, 0.f, 45.f)),
		Scale(glm::vec3(500.f, 100.f, 500.f)),
	});
		meshes[(int)GeoType::Terrain]->SetModel(GetTopModel());
		meshes[(int)GeoType::Terrain]->Render(geoPassSP);
	PopModel();

	///Shapes
	PushModel({
		Translate(glm::vec3(0.f, 100.f, 0.f)),
		Rotate(glm::vec4(0.f, 1.f, 0.f, 0.f)),
		Scale(glm::vec3(10.f)),
	});
		//geoPassSP.Set1i("useCustomColour", 1);
		//geoPassSP.Set4fv("customColour", glm::vec4(20.f, 60.f, 20.f, 1.f));
		meshes[(int)GeoType::Cylinder]->SetModel(GetTopModel());
		meshes[(int)GeoType::Cylinder]->Render(geoPassSP);
		//geoPassSP.Set1i("useCustomColour", 0);
		PushModel({
			Translate(glm::vec3(-3.f, 0.f, 0.f)),
		});
			meshes[(int)GeoType::Sphere]->SetModel(GetTopModel());
			meshes[(int)GeoType::Sphere]->Render(geoPassSP);
		PopModel();
		PushModel({
			Translate(glm::vec3(3.f, 0.f, 0.f)),
		});
			meshes[(int)GeoType::Cube]->SetModel(GetTopModel());
			meshes[(int)GeoType::Cube]->Render(geoPassSP);
		PopModel();
		PushModel({
			Translate(glm::vec3(6.f, 0.f, 0.f)),
		});
			meshes[(int)GeoType::Quad]->SetModel(GetTopModel());
			meshes[(int)GeoType::Quad]->Render(geoPassSP);
		PopModel();
	PopModel();

	///SpriteAni
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
	PushModel({
		Translate(glm::vec3(0.f, 50.f, 0.f)),
		Scale(glm::vec3(20.f, 40.f, 20.f)),
	});
		meshes[(int)GeoType::SpriteAni]->SetModel(GetTopModel());
		meshes[(int)GeoType::SpriteAni]->Render(geoPassSP);
	PopModel();
	glBlendFunc(GL_ONE, GL_ZERO);  
}

void Scene::LightingRenderPass(const uint& posTexRefID, const uint& coloursTexRefID, const uint& normalsTexRefID, const uint& specTexRefID, const uint& reflectionTexRefID){
	lightingPassSP.Use();
	const int& pAmt = (int)ptLights.size();
	const int& dAmt = (int)directionalLights.size();
	const int& sAmt = (int)spotlights.size();
	lightingPassSP.Set1f("mtl.shininess", 32.f); //More light scattering if lower
	lightingPassSP.Set3fv("globalAmbient", Light::globalAmbient);

	lightingPassSP.Set3fv("camPos", cam.GetPos());
	lightingPassSP.UseTex(posTexRefID, "posTex");
	lightingPassSP.UseTex(coloursTexRefID, "coloursTex");
	lightingPassSP.UseTex(normalsTexRefID, "normalsTex");
	lightingPassSP.UseTex(specTexRefID, "specTex");
	lightingPassSP.UseTex(reflectionTexRefID, "reflectionTex");

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

	meshes[(int)GeoType::Quad]->SetModel(GetTopModel());
	meshes[(int)GeoType::Quad]->Render(lightingPassSP, false);
	lightingPassSP.ResetTexUnits();
}

void Scene::BlurRender(const uint& brightTexRefID, const bool& horizontal){
	blurSP.Use();
	blurSP.Set1i("horizontal", horizontal);
	blurSP.UseTex(brightTexRefID, "texSampler");
	meshes[(int)GeoType::Quad]->SetModel(GetTopModel());
	meshes[(int)GeoType::Quad]->Render(blurSP, false);
	blurSP.ResetTexUnits();
}

void Scene::DefaultRender(const uint& screenTexRefID, const uint& blurTexRefID){
	screenSP.Use();
	screenSP.UseTex(screenTexRefID, "screenTexSampler");
	screenSP.UseTex(blurTexRefID, "blurTexSampler");
	meshes[(int)GeoType::Quad]->SetModel(GetTopModel());
	meshes[(int)GeoType::Quad]->Render(screenSP, false);
	screenSP.ResetTexUnits();

	glDepthFunc(GL_LEQUAL);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	textChief.RenderText(textSP, {
		"Wow",
		25.f,
		25.f,
		1.f,
		glm::vec4(1.f),
		0
	});
	glBlendFunc(GL_ONE, GL_ZERO);
	glDepthFunc(GL_LESS);
}

glm::mat4 Scene::Translate(const glm::vec3& translate){
	return glm::translate(glm::mat4(1.f), translate);
}

glm::mat4 Scene::Rotate(const glm::vec4& rotate){
	return glm::rotate(glm::mat4(1.f), glm::radians(rotate.w), glm::vec3(rotate));
}

glm::mat4 Scene::Scale(const glm::vec3& scale){
	return glm::scale(glm::mat4(1.f), scale);
}

glm::mat4 Scene::GetTopModel() const{
	return modelStack.empty() ? glm::mat4(1.f) : modelStack.top();
}

void Scene::PushModel(const std::vector<glm::mat4>& vec) const{
	modelStack.push(modelStack.empty() ? glm::mat4(1.f) : modelStack.top());
	const size_t size = vec.size();
	for(size_t i = 0; i < size; ++i){
		modelStack.top() *= vec[i];
	}
}

void Scene::PopModel() const{
	modelStack.pop();
}