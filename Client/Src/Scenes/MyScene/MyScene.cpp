#include "MyScene.h"

#include "Vendor/stb_image.h"
#include "Geo/Model.h"
#include "Geo/SpriteAni.h"
#include "Geo/Terrain.h"
#include "Math/Pseudorand.h"

extern float angularFOV;
extern int winWidth;
extern int winHeight;

glm::vec3 Light::globalAmbient = glm::vec3(.2f);

MyScene::MyScene():
	cam(glm::vec3(0.f, 0.f, 5.f), glm::vec3(0.f), 0.f, 150.f),
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
			{"Imgs/BoxAlbedo.png", Mesh::TexType::Diffuse, 0},
		}),
		new Mesh(Mesh::MeshType::Sphere, GL_TRIANGLE_STRIP, {
			{"Imgs/Skydome.hdr", Mesh::TexType::Diffuse, 0},
		}),
		new Mesh(Mesh::MeshType::Cylinder, GL_TRIANGLE_STRIP, {
			{"Imgs/BoxAlbedo.png", Mesh::TexType::Diffuse, 0},
		}),
		new SpriteAni(4, 8),
		new Terrain("Imgs/hMap.raw", 8.f, 8.f),
	},
	models{
		new Model("ObjsAndMtls/Skydome.obj", {
			aiTextureType_DIFFUSE,
		}),
		new Model("ObjsAndMtls/nanosuit.obj", {
			aiTextureType_DIFFUSE,
			aiTextureType_SPECULAR,
			aiTextureType_EMISSIVE,
			aiTextureType_AMBIENT,
			//aiTextureType_HEIGHT,
		}),
	},
	blurSP{"Shaders/Quad.vs", "Shaders/Blur.fs"},
	forwardSP(nullptr),
	geoPassSP(nullptr),
	lightingPassSP{"Shaders/Quad.vs", "Shaders/LightingPass.fs"},
	normalsSP{"Shaders/Normals.vs", "Shaders/Normals.fs", "Shaders/Normals.gs"}, //??
	screenSP{"Shaders/Quad.vs", "Shaders/Screen.fs"},
	textSP{"Shaders/Text.vs", "Shaders/Text.fs"},
	ptLights({}),
	directionalLights({}),
	spotlights({}),
	view(glm::mat4(1.f)),
	projection(glm::mat4(1.f)),
	FPS(0.0f),
	elapsedTime(0.f),
	polyMode(0),
	modelStack()
{
}

MyScene::~MyScene(){
	if(forwardSP != nullptr){
		delete forwardSP;
		forwardSP = nullptr;
	}

	if(geoPassSP != nullptr){
		delete geoPassSP;
		geoPassSP = nullptr;
	}

	const size_t& pSize = ptLights.size();
	const size_t& dSize = directionalLights.size();
	const size_t& sSize = spotlights.size();
	for(size_t i = 0; i < pSize; ++i){
		if(ptLights[i]){
			delete ptLights[i];
			ptLights[i] = nullptr;
		}
	}
	for(size_t i = 0; i < dSize; ++i){
		if(directionalLights[i]){
			delete directionalLights[i];
			directionalLights[i] = nullptr;
		}
	}
	for(size_t i = 0; i < sSize; ++i){
		if(spotlights[i]){
			delete spotlights[i];
			spotlights[i] = nullptr;
		}
	}
	
	for(int i = 0; i < (int)MeshType::Amt; ++i){
		if(meshes[i]){
			delete meshes[i];
			meshes[i] = nullptr;
		}
	}
	for(int i = 0; i < (int)ModelType::Amt; ++i){
		if(models[i]){
			delete models[i];
			models[i] = nullptr;
		}
	}
	if(music){
		music->drop();
	}
	if(soundEngine){
		soundEngine->drop();
	}
}

bool MyScene::Init(){
	glGetIntegerv(GL_POLYGON_MODE, &polyMode);


	int maxTexImgUnits;
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTexImgUnits);

	if(maxTexImgUnits >= 32){
		forwardSP = new ShaderProg("Shaders/Forward.vs", "Shaders/Forward28.fs");
		geoPassSP = new ShaderProg("Shaders/GeoPass.vs", "Shaders/GeoPass28.fs");
	} else{
		forwardSP = new ShaderProg("Shaders/Forward.vs", "Shaders/Forward12.fs");
		geoPassSP = new ShaderProg("Shaders/GeoPass.vs", "Shaders/GeoPass12.fs");
	}


	soundEngine = createIrrKlangDevice(ESOD_AUTO_DETECT, ESEO_DEFAULT_OPTIONS & ~ESEO_PRINT_DEBUG_INFO_TO_DEBUGGER);
	if(!soundEngine){
		(void)puts("Failed to init soundEngine!\n");
	}
	soundEngine->setSoundVolume(0.0f);
	//soundEngine->play2D("Audio/Music/YellowCafe.mp3", true);

	music = soundEngine->play3D("Audio/Music/YellowCafe.mp3", vec3df(0.f, 0.f, 0.f), true, true, true, ESM_AUTO_DETECT, true);
	if(music){
		music->setMinDistance(5.f);
		music->setVolume(0.0f);

		soundFX = music->getSoundEffectControl();
		if(!soundFX){
			(void)puts("No soundFX support!\n");
		}
	} else{
		(void)puts("Failed to init music!\n");
	}

	soundEngine->setRolloffFactor(10.f);
	soundEngine->setDopplerEffectParameters(10.f, 10.f);

	for(int i = 0; i < 99; ++i){
		modelStack.PushModel({
			modelStack.Translate(glm::vec3(PseudorandMinMax(-2000.f, 2000.f), PseudorandMinMax(-2000.f, 2000.f), -5.f)),
			modelStack.Rotate(glm::vec4(0.f, 1.f, 0.f, -45.f)),
		});
			meshes[(int)MeshType::Quad]->AddModelMat4(modelStack.GetTopModel());
		modelStack.PopModel();
	}
	for(int i = 0; i < 999; ++i){
		modelStack.PushModel({
			modelStack.Translate(glm::vec3(PseudorandMinMax(-100.f, 100.f), PseudorandMinMax(-100.f, 100.f), -5.f)),
			modelStack.Rotate(glm::vec4(0.f, 1.f, 0.f, -45.f)),
		});
			models[(int)ModelType::Suit]->AddModelMat4ForAll(modelStack.GetTopModel());
		modelStack.PopModel();
	}

	meshes[(int)MeshType::SpriteAni]->AddTexMap({"Imgs/Fire.png", Mesh::TexType::Diffuse, 0});
	static_cast<SpriteAni*>(meshes[(int)MeshType::SpriteAni])->AddAni("FireSpriteAni", 0, 32);
	static_cast<SpriteAni*>(meshes[(int)MeshType::SpriteAni])->Play("FireSpriteAni", -1, .5f);

	meshes[(int)MeshType::Terrain]->AddTexMap({"Imgs/GrassGround.jpg", Mesh::TexType::Diffuse, 0});

	spotlights.emplace_back(CreateLight(LightType::Spot));

	return true;
}

void MyScene::Update(const float dt){
	FPS = 1.0f / dt;
	elapsedTime += dt;
	if(winHeight){ //Avoid division by 0 when win is minimised
		cam.SetDefaultAspectRatio(float(winWidth) / float(winHeight));
		cam.ResetAspectRatio();
	}
	cam.Update(dt, GLFW_KEY_Q, GLFW_KEY_E, GLFW_KEY_A, GLFW_KEY_D, GLFW_KEY_W, GLFW_KEY_S);
	view = cam.LookAt();
	projection = glm::perspective(glm::radians(angularFOV), cam.GetAspectRatio(), .1f, 9999.f);

	const glm::vec3& camWorldSpacePos = cam.GetPos();
	const glm::vec3& camFront = cam.CalcFront();
	soundEngine->setListenerPosition(vec3df(camWorldSpacePos.x, camWorldSpacePos.y, camWorldSpacePos.z), vec3df(camFront.x, camFront.y, camFront.z));

	spotlights[0]->ambient = glm::vec3(.05f);
	spotlights[0]->diffuse = glm::vec3(.8f);
	spotlights[0]->spec = glm::vec3(1.f);
	static_cast<Spotlight*>(spotlights[0])->pos = camWorldSpacePos;
	static_cast<Spotlight*>(spotlights[0])->dir = camFront;
	static_cast<Spotlight*>(spotlights[0])->cosInnerCutoff = cosf(glm::radians(12.5f));
	static_cast<Spotlight*>(spotlights[0])->cosOuterCutoff = cosf(glm::radians(17.5f));

	static_cast<SpriteAni*>(meshes[(int)MeshType::SpriteAni])->Update(dt);

	static float polyModeBT = 0.f;
	static float distortionBT = 0.f;
	static float echoBT = 0.f;
	static float wavesReverbBT = 0.f;
	static float resetSoundFXBT = 0.f;

	if(Key(VK_F2) && polyModeBT <= elapsedTime){
		polyMode += polyMode == GL_FILL ? -2 : 1;
		glPolygonMode(GL_FRONT_AND_BACK, polyMode);
		polyModeBT = elapsedTime + .5f;
	}

	//for(int i = 0; i < 99999; ++i){
	//	PushModel({
	//		Translate(glm::vec3(PseudorandMinMax(-2000.f, 2000.f), PseudorandMinMax(-2000.f, 2000.f), -5.f)),
	//		Rotate(glm::vec4(0.f, 1.f, 0.f, -45.f)),
	//	});
	//		meshes[(int)MeshType::Quad]->SetModelMat(GetTopModel(), i);
	//	PopModel();
	//}

	if(soundFX){
		if(Key(GLFW_KEY_I) && distortionBT <= elapsedTime){
			soundFX->isDistortionSoundEffectEnabled() ? soundFX->disableDistortionSoundEffect() : (void)soundFX->enableDistortionSoundEffect();
			distortionBT = elapsedTime + .5f;
		}
		if(Key(GLFW_KEY_O) && echoBT <= elapsedTime){
			soundFX->isEchoSoundEffectEnabled() ? soundFX->disableEchoSoundEffect() : (void)soundFX->enableEchoSoundEffect();
			echoBT = elapsedTime + .5f;
		}
		if(Key(GLFW_KEY_P) && wavesReverbBT <= elapsedTime){
			soundFX->isWavesReverbSoundEffectEnabled() ? soundFX->disableWavesReverbSoundEffect() : (void)soundFX->enableWavesReverbSoundEffect();
			wavesReverbBT = elapsedTime + .5f;
		}
		if(Key(GLFW_KEY_L) && resetSoundFXBT <= elapsedTime){
			soundFX->disableAllEffects();
			resetSoundFXBT = elapsedTime + .5f;
		}
	}
}

void MyScene::GeoRenderPass(){
	geoPassSP->Use();
	geoPassSP->SetMat4fv("PV", &(projection * glm::mat4(glm::mat3(view)))[0][0]);

	///Sky
	glDepthFunc(GL_LEQUAL); //Modify comparison operators used for depth test such that frags with depth <= 1.f are shown
	glCullFace(GL_FRONT);
	geoPassSP->Set1i("sky", 1);
	modelStack.PushModel({
		modelStack.Rotate(glm::vec4(0.f, 1.f, 0.f, glfwGetTime())),
	});
		meshes[(int)MeshType::Sphere]->SetModelMat4(modelStack.GetTopModel());
		meshes[(int)MeshType::Sphere]->Render(*geoPassSP);
	modelStack.PopModel();
	geoPassSP->Set1i("sky", 0);
	glCullFace(GL_BACK);
	glDepthFunc(GL_LESS);

	geoPassSP->SetMat4fv("PV", &(projection * view)[0][0]);

	///Terrain
	modelStack.PushModel({
		modelStack.Scale(glm::vec3(500.f, 100.f, 500.f)),
	});
		meshes[(int)MeshType::Terrain]->SetModelMat4(modelStack.GetTopModel());
		meshes[(int)MeshType::Terrain]->Render(*geoPassSP);
	modelStack.PopModel();

	///Nanosuits
	modelStack.PushModel({
		modelStack.Translate(glm::vec3(0.f, 200.f, 0.f)),
		modelStack.Rotate(glm::vec4(0.f, 1.f, 0.f, 0.f)),
		modelStack.Scale(glm::vec3(5.f)),
	});
		models[(int)ModelType::Suit]->SetModelMat4ForAll(modelStack.GetTopModel());
		models[(int)ModelType::Suit]->Render(*geoPassSP);
	modelStack.PopModel();
	modelStack.PushModel({
		modelStack.Translate(glm::vec3(0.f, 100.f, 0.f)),
		modelStack.Rotate(glm::vec4(0.f, 1.f, 0.f, 0.f)),
		modelStack.Scale(glm::vec3(.5f)),
	});
		models[(int)ModelType::Suit]->SetModelMat4ForAll(modelStack.GetTopModel());
		models[(int)ModelType::Suit]->InstancedRender(*geoPassSP);
	modelStack.PopModel();

	///Shapes
	modelStack.PushModel({
		modelStack.Translate(glm::vec3(0.f, 100.f, 0.f)),
		modelStack.Scale(glm::vec3(10.f)),
	});
		modelStack.PushModel({
			modelStack.Translate(glm::vec3(6.f, 0.f, 0.f)),
		});
			geoPassSP->Set1i("noNormals", 1);
			geoPassSP->Set1i("useCustomColour", 1);
			geoPassSP->Set4fv("customColour", glm::vec4(glm::vec3(5.f), 1.f));
			meshes[(int)MeshType::Quad]->SetModelMat4(modelStack.GetTopModel());
			meshes[(int)MeshType::Quad]->Render(*geoPassSP);
			geoPassSP->Set1i("useCustomColour", 0);
			geoPassSP->Set1i("noNormals", 0);
			modelStack.PushModel({
				modelStack.Translate(glm::vec3(0.f, 0.f, 5.f)),
			});
				meshes[(int)MeshType::Sphere]->SetModelMat4(modelStack.GetTopModel());
				meshes[(int)MeshType::Sphere]->Render(*geoPassSP);
			modelStack.PopModel();
			modelStack.PushModel({
				modelStack.Translate(glm::vec3(0.f, 0.f, -5.f)),
			});
				meshes[(int)MeshType::Cylinder]->SetModelMat4(modelStack.GetTopModel());
				meshes[(int)MeshType::Cylinder]->Render(*geoPassSP);
			modelStack.PopModel();
		modelStack.PopModel();
	modelStack.PopModel();
}

void MyScene::LightingRenderPass(
	const uint& posTexRefID,
	const uint& coloursTexRefID,
	const uint& normalsTexRefID,
	const uint& specTexRefID,
	const uint& reflectionTexRefID
){
	lightingPassSP.Use();
	const int& pAmt = (int)ptLights.size();
	const int& dAmt = (int)directionalLights.size();
	const int& sAmt = (int)spotlights.size();

	lightingPassSP.Set1f("shininess", 32.f); //More light scattering if lower
	lightingPassSP.Set3fv("globalAmbient", Light::globalAmbient);
	lightingPassSP.Set3fv("camWorldSpacePos", cam.GetPos());
	lightingPassSP.Set1i("pAmt", pAmt);
	lightingPassSP.Set1i("dAmt", dAmt);
	lightingPassSP.Set1i("sAmt", sAmt);
	lightingPassSP.UseTex("posTex", posTexRefID);
	lightingPassSP.UseTex("coloursTex", coloursTexRefID);
	lightingPassSP.UseTex("normalsTex", normalsTexRefID);
	lightingPassSP.UseTex("specTex", specTexRefID);
	lightingPassSP.UseTex("reflectionTex", reflectionTexRefID);

	int i;
	for(i = 0; i < pAmt; ++i){
		const PtLight* const& ptLight = static_cast<PtLight*>(ptLights[i]);
		lightingPassSP.Set3fv(("ptLights[" + std::to_string(i) + "].ambient").c_str(), ptLight->ambient);
		lightingPassSP.Set3fv(("ptLights[" + std::to_string(i) + "].diffuse").c_str(), ptLight->diffuse);
		lightingPassSP.Set3fv(("ptLights[" + std::to_string(i) + "].spec").c_str(), ptLight->spec);
		lightingPassSP.Set3fv(("ptLights[" + std::to_string(i) + "].pos").c_str(), ptLight->pos);
		lightingPassSP.Set1f(("ptLights[" + std::to_string(i) + "].constant").c_str(), ptLight->constant);
		lightingPassSP.Set1f(("ptLights[" + std::to_string(i) + "].linear").c_str(), ptLight->linear);
		lightingPassSP.Set1f(("ptLights[" + std::to_string(i) + "].quadratic").c_str(), ptLight->quadratic);
	}
	for(i = 0; i < dAmt; ++i){
		const DirectionalLight* const& directionalLight = static_cast<DirectionalLight*>(directionalLights[i]);
		lightingPassSP.Set3fv(("directionalLights[" + std::to_string(i) + "].ambient").c_str(), directionalLight->ambient);
		lightingPassSP.Set3fv(("directionalLights[" + std::to_string(i) + "].diffuse").c_str(), directionalLight->diffuse);
		lightingPassSP.Set3fv(("directionalLights[" + std::to_string(i) + "].spec").c_str(), directionalLight->spec);
		lightingPassSP.Set3fv(("directionalLights[" + std::to_string(i) + "].dir").c_str(), directionalLight->dir);
	}
	for(i = 0; i < sAmt; ++i){
		const Spotlight* const& spotlight = static_cast<Spotlight*>(spotlights[i]);
		lightingPassSP.Set3fv(("spotlights[" + std::to_string(i) + "].ambient").c_str(), spotlight->ambient);
		lightingPassSP.Set3fv(("spotlights[" + std::to_string(i) + "].diffuse").c_str(), spotlight->diffuse);
		lightingPassSP.Set3fv(("spotlights[" + std::to_string(i) + "].spec").c_str(), spotlight->spec);
		lightingPassSP.Set3fv(("spotlights[" + std::to_string(i) + "].pos").c_str(), spotlight->pos);
		lightingPassSP.Set3fv(("spotlights[" + std::to_string(i) + "].dir").c_str(), spotlight->dir);
		lightingPassSP.Set1f(("spotlights[" + std::to_string(i) + "].cosInnerCutoff").c_str(), spotlight->cosInnerCutoff);
		lightingPassSP.Set1f(("spotlights[" + std::to_string(i) + "].cosOuterCutoff").c_str(), spotlight->cosOuterCutoff);
	}

	meshes[(int)MeshType::Quad]->SetModelMat4(modelStack.GetTopModel());
	meshes[(int)MeshType::Quad]->Render(lightingPassSP, false);
	lightingPassSP.ResetTexUnits();
}

void MyScene::BlurRender(const uint& brightTexRefID, const bool& horizontal){
	blurSP.Use();
	blurSP.Set1i("horizontal", horizontal);
	blurSP.UseTex("texSampler", brightTexRefID);
	meshes[(int)MeshType::Quad]->SetModelMat4(modelStack.GetTopModel());
	meshes[(int)MeshType::Quad]->Render(blurSP, false);
	blurSP.ResetTexUnits();
}

void MyScene::DefaultRender(const uint& screenTexRefID, const uint& blurTexRefID){
	screenSP.Use();
	screenSP.Set1f("exposure", 1.2f);
	screenSP.UseTex("screenTexSampler", screenTexRefID);
	screenSP.UseTex("blurTexSampler", blurTexRefID);
	meshes[(int)MeshType::Quad]->SetModelMat4(modelStack.GetTopModel());
	meshes[(int)MeshType::Quad]->Render(screenSP, false);
	screenSP.ResetTexUnits();
}

void MyScene::ForwardRender(){
	forwardSP->Use();

	const int& pAmt = 0;
	const int& dAmt = 0;
	const int& sAmt = 0;

	forwardSP->Set1f("shininess", 32.f); //More light scattering if lower
	forwardSP->Set3fv("globalAmbient", Light::globalAmbient);
	forwardSP->Set3fv("camWorldSpacePos", cam.GetPos());
	forwardSP->Set1i("pAmt", pAmt);
	forwardSP->Set1i("dAmt", dAmt);
	forwardSP->Set1i("sAmt", sAmt);

	int i;
	for(i = 0; i < pAmt; ++i){
		const PtLight* const& ptLight = static_cast<PtLight*>(ptLights[i]);
		forwardSP->Set3fv(("ptLights[" + std::to_string(i) + "].ambient").c_str(), ptLight->ambient);
		forwardSP->Set3fv(("ptLights[" + std::to_string(i) + "].diffuse").c_str(), ptLight->diffuse);
		forwardSP->Set3fv(("ptLights[" + std::to_string(i) + "].spec").c_str(), ptLight->spec);
		forwardSP->Set3fv(("ptLights[" + std::to_string(i) + "].pos").c_str(), ptLight->pos);
		forwardSP->Set1f(("ptLights[" + std::to_string(i) + "].constant").c_str(), ptLight->constant);
		forwardSP->Set1f(("ptLights[" + std::to_string(i) + "].linear").c_str(), ptLight->linear);
		forwardSP->Set1f(("ptLights[" + std::to_string(i) + "].quadratic").c_str(), ptLight->quadratic);
	}
	for(i = 0; i < dAmt; ++i){
		const DirectionalLight* const& directionalLight = static_cast<DirectionalLight*>(directionalLights[i]);
		forwardSP->Set3fv(("directionalLights[" + std::to_string(i) + "].ambient").c_str(), directionalLight->ambient);
		forwardSP->Set3fv(("directionalLights[" + std::to_string(i) + "].diffuse").c_str(), directionalLight->diffuse);
		forwardSP->Set3fv(("directionalLights[" + std::to_string(i) + "].spec").c_str(), directionalLight->spec);
		forwardSP->Set3fv(("directionalLights[" + std::to_string(i) + "].dir").c_str(), directionalLight->dir);
	}
	for(i = 0; i < sAmt; ++i){
		const Spotlight* const& spotlight = static_cast<Spotlight*>(spotlights[i]);
		forwardSP->Set3fv(("spotlights[" + std::to_string(i) + "].ambient").c_str(), spotlight->ambient);
		forwardSP->Set3fv(("spotlights[" + std::to_string(i) + "].diffuse").c_str(), spotlight->diffuse);
		forwardSP->Set3fv(("spotlights[" + std::to_string(i) + "].spec").c_str(), spotlight->spec);
		forwardSP->Set3fv(("spotlights[" + std::to_string(i) + "].pos").c_str(), spotlight->pos);
		forwardSP->Set3fv(("spotlights[" + std::to_string(i) + "].dir").c_str(), spotlight->dir);
		forwardSP->Set1f(("spotlights[" + std::to_string(i) + "].cosInnerCutoff").c_str(), spotlight->cosInnerCutoff);
		forwardSP->Set1f(("spotlights[" + std::to_string(i) + "].cosOuterCutoff").c_str(), spotlight->cosOuterCutoff);
	}

	forwardSP->SetMat4fv("PV", &(projection * view)[0][0]);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	///Shapes
	modelStack.PushModel({
		modelStack.Translate(glm::vec3(0.f, 100.f, 0.f)),
		modelStack.Scale(glm::vec3(10.f)),
	});
		forwardSP->Set1i("useCustomColour", 1);
		forwardSP->Set4fv("customColour", glm::vec4(glm::rgbColor(glm::vec3(1.f, PseudorandMinMax(0.f, 255.f), 1.f)) * .5f, .5f));
		meshes[(int)MeshType::Cylinder]->SetModelMat4(modelStack.GetTopModel());
		meshes[(int)MeshType::Cylinder]->Render(*forwardSP);
		forwardSP->Set1i("useCustomColour", 0);
		modelStack.PushModel({
			modelStack.Translate(glm::vec3(-3.f, 0.f, 0.f)),
		});
			forwardSP->Set1i("useCustomColour", 1);
			forwardSP->Set4fv("customColour", glm::vec4(glm::rgbColor(glm::vec3(1.f, 1.f, PseudorandMinMax(0.f, 255.f))) * 7.f, .3f));
			forwardSP->Set1i("useCustomDiffuseTexIndex", 1);
			forwardSP->Set1i("customDiffuseTexIndex", -1);
			meshes[(int)MeshType::Sphere]->SetModelMat4(modelStack.GetTopModel());
			meshes[(int)MeshType::Sphere]->Render(*forwardSP);
			forwardSP->Set1i("useCustomDiffuseTexIndex", 0);
			forwardSP->Set1i("useCustomColour", 0);
		modelStack.PopModel();
		modelStack.PushModel({
			modelStack.Translate(glm::vec3(3.f, 0.f, 0.f)),
		});
			forwardSP->Set1i("useCustomColour", 1);
			forwardSP->Set4fv("customColour", glm::vec4(glm::rgbColor(glm::vec3(PseudorandMinMax(0.f, 255.f), 1.f, 1.f)) * .5f, .7f));
			forwardSP->Set1i("useCustomDiffuseTexIndex", 1);
			forwardSP->Set1i("customDiffuseTexIndex", -1);
			meshes[(int)MeshType::Cube]->SetModelMat4(modelStack.GetTopModel());
			meshes[(int)MeshType::Cube]->Render(*forwardSP);
			forwardSP->Set1i("useCustomDiffuseTexIndex", 0);
			forwardSP->Set1i("useCustomColour", 0);
		modelStack.PopModel();
	modelStack.PopModel();

	///SpriteAni
	modelStack.PushModel({
		modelStack.Translate(glm::vec3(0.f, 50.f, 0.f)),
		modelStack.Scale(glm::vec3(20.f, 40.f, 20.f)),
	});
		forwardSP->Set1i("noNormals", 1);
		forwardSP->Set1i("useCustomColour", 1);
		forwardSP->Set4fv("customColour", glm::vec4(glm::vec3(1.f), 1.f));
		meshes[(int)MeshType::SpriteAni]->SetModelMat4(modelStack.GetTopModel());
		meshes[(int)MeshType::SpriteAni]->Render(*forwardSP);
		forwardSP->Set1i("useCustomColour", 0);
		forwardSP->Set1i("noNormals", 0);
	modelStack.PopModel();

	textChief.RenderText(textSP, {
		(str)"FPS: " + std::to_string(FPS).substr(0, std::to_string((int)FPS).length() + 3),
		(float)winWidth,
		225.f,
		1.f,
		glm::vec4(1.f),
		0,
		TextChief::TextAlignment::Right
	});
	textChief.RenderText(textSP, {
		"AAAA",
		0.0f,
		150.f,
		1.f,
		glm::vec4(1.f),
		0,
		TextChief::TextAlignment::Left
	});
	textChief.RenderText(textSP, {
		"AAAAAA",
		(float)winWidth / 2.f,
		75.f,
		1.f,
		glm::vec4(1.f),
		0,
		TextChief::TextAlignment::Center
	});

	glBlendFunc(GL_ONE, GL_ZERO);

	if(music && music->getIsPaused()){
		music->setIsPaused(false);
	}
}