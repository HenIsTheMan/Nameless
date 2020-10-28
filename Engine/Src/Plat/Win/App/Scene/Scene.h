#pragma once

//class App;

class Scene final{
	//friend App;
public:
	Scene();
	~Scene() = default;
	Scene(const Scene& scene) = default;
	Scene(Scene&& scene) noexcept = default;
	Scene& operator=(const Scene& scene) = default;
	Scene& operator=(Scene&& scene) noexcept = default;

	///Setters
	void SetInit(void (*init)());
	void SetFixedUpdate(void (*fixedUpdate)(float dt));
	void SetUpdate(void (*update)(float dt));
	void SetLateUpdate(void (*lateUpdate)(float dt));
	void SetPreRender(void (*preRender)());
	void SetRender(void (*render)());
	void SetPostRender(void (*postRender)());
//private:
	void (*im_Init)();
	void (*im_FixedUpdate)(float dt);
	void (*im_Update)(float dt);
	void (*im_LateUpdate)(float dt);
	void (*im_PreRender)();
	void (*im_Render)();
	void (*im_PostRender)();

	void Init();
	void FixedUpdate(float dt);
	void Update(float dt);
	void LateUpdate(float dt);
	void PreRender();
	void Render();
	void PostRender();
};