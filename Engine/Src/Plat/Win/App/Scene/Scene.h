#pragma once

class App;

class Scene final{
	friend App;
public:
	Scene();
	~Scene() = default;
	Scene(const Scene&) = default;
	Scene(Scene&&) noexcept = default;
	Scene& operator=(const Scene&) = default;
	Scene& operator=(Scene&&) noexcept = default;

	///Setters
	void SetInCtor(void (*inCtor)());
	void SetInDtor(void (*inDtor)());
	void SetInit(void (*init)());
	void SetFixedUpdate(void (*fixedUpdate)(float dt));
	void SetUpdate(void (*update)(float dt));
	void SetLateUpdate(void (*lateUpdate)(float dt));
	void SetPreRender(void (*preRender)());
	void SetRender(void (*render)());
	void SetPostRender(void (*postRender)());
private:
	void (*im_InCtor)();
	void (*im_InDtor)();
	void (*im_Init)();
	void (*im_FixedUpdate)(float dt);
	void (*im_Update)(float dt);
	void (*im_LateUpdate)(float dt);
	void (*im_PreRender)();
	void (*im_Render)();
	void (*im_PostRender)();

	void InCtor() const;
	void InDtor() const;
	void Init() const;
	void FixedUpdate(float dt) const;
	void Update(float dt) const;
	void LateUpdate(float dt) const;
	void PreRender() const;
	void Render() const;
	void PostRender() const;
};