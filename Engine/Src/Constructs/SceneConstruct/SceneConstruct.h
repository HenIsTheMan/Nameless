#pragma once

class SceneConstruct final{ //Static class
	SceneConstruct() = delete;
	~SceneConstruct() = delete;
	SceneConstruct(const SceneConstruct&) = delete;
	SceneConstruct(SceneConstruct&&) noexcept = delete;
	SceneConstruct& operator=(const SceneConstruct&) = delete;
	SceneConstruct& operator=(SceneConstruct&&) noexcept = delete;
public:
	static void SetInCtor(void (*inCtor)());
	static void SetInDtor(void (*inDtor)());
	static void SetInit(void (*init)());
	static void SetFixedUpdate(void (*fixedUpdate)(const float dt));
	static void SetUpdate(void (*update)(const float dt));
	static void SetLateUpdate(void (*lateUpdate)(const float dt));
	static void SetPreRender(void (*preRender)());
	static void SetRender(void (*render)());
	static void SetPostRender(void (*postRender)());

	static void InCtor();
	static void InDtor();
	static void Init();
	static void FixedUpdate(const float dt);
	static void Update(const float dt);
	static void LateUpdate(const float dt);
	static void PreRender();
	static void Render();
	static void PostRender();
private:
	static void (*sm_InCtor)();
	static void (*sm_InDtor)();
	static void (*sm_Init)();
	static void (*sm_FixedUpdate)(const float dt);
	static void (*sm_Update)(const float dt);
	static void (*sm_LateUpdate)(const float dt);
	static void (*sm_PreRender)();
	static void (*sm_Render)();
	static void (*sm_PostRender)();
};