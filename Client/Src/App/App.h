#pragma once
#include <Engine.h>
#include "../World/ShaderProg.h" //??

class App final: public Singleton<App>{
	App(const App&) = delete;
	App(App&&) = delete;
	App& operator=(const App&) = delete;
	App& operator=(App&&) noexcept = delete;

	friend Singleton<App>;
	App() = default;
	static GLFWwindow* win;
public:
	~App();
	bool Init();
	void Update();
	void PreRender() const;
	void Render();
	void PostRender() const;



	ShaderProg shaderProg;
	Mesh mesh;
};