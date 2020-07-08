#pragma once
#include "../World/Scene.h"

class App final: public Singleton<App>{
public:
	enum struct FBO{
		GeoPass = 0,
		LightingPass,
	};
	enum struct Tex{
		Pos = 0,
		Normals,
		AlbedoSpecular,
		Lit,
		BrightLit,
	};
	~App();
	bool Init();
	void Update();
	void PreRender() const;
	void Render();
	void PostRender() const;
private:
	explicit App(const App&) = delete;
	explicit App(App&&) = delete;
	App& operator=(const App&) = delete;
	App& operator=(App&&) noexcept = delete;

	friend Singleton<App>;
	App();
	bool InitOptions() const;
	float lastFrameTime;
	Scene scene;
	static GLFWwindow* win;

	uint FBORefIDs[2];
	uint texRefIDs[5];
	uint RBORefIDs[2];
};

template <class T>
inline T& operator++(T& myEnum){
	myEnum = T((int)myEnum + 1);
	return myEnum;
}