#pragma once
#include "../World/Scene.h"

class App final: public Singleton<App>{
public:
	enum struct FBO{
		GeoPass = 0,
		LightingPass,
		Amt
	};
	enum struct Tex{
		Pos = 0,
		Normals,
		AlbedoSpec,
		Lit,
		BrightLit,
		Amt
	};
	~App();
	bool Init();
	void Update();
	void PreRender() const;
	void Render();
	void PostRender() const;
private:
	explicit App(const App&) = delete;
	explicit App(App&&) noexcept = delete;
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
	uint RBORefIDs[1];
};

template <class T>
inline T& operator++(T& myEnum){
	myEnum = T((int)myEnum + 1);
	return myEnum;
}