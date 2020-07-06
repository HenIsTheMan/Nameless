#pragma once
#include "../World/Scene.h"

class App final: public Singleton<App>{
	explicit App(const App&) = delete;
	explicit App(App&&) = delete;
	App& operator=(const App&) = delete;
	App& operator=(App&&) noexcept = delete;

	friend Singleton<App>;
	App();
	float lastFrameTime;
	Scene scene;
	static GLFWwindow* win;
public:
	~App();
	void Update();
	void PreRender() const;
	void Render();
	void PostRender() const;



	uint gBufferRefID;
	uint texRefIDs[3];
	uint RBORefID;
};