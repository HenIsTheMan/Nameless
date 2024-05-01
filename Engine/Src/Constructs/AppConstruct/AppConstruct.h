#pragma once

static void MainProcess(); //Forward declaration

class AppConstruct final{ //Static class
	AppConstruct() = delete;
	~AppConstruct() = delete;
	AppConstruct(const AppConstruct&) = delete;
	AppConstruct(AppConstruct&&) noexcept = delete;
	AppConstruct& operator=(const AppConstruct&) = delete;
	AppConstruct& operator=(AppConstruct&&) noexcept = delete;

	friend static void MainProcess();
public:
	static void SetInCtor(void (*inCtor)());
	static void SetInDtor(void (*inDtor)());
	static void SetInit(void (*init)());
	static void SetUpdate(void (*update)(const float dt));
	static void SetRender(void (*render)());
private:
	static void (*sm_InCtor)();
	static void (*sm_InDtor)();
	static void (*sm_Init)();
	static void (*sm_Update)(const float dt);
	static void (*sm_Render)();

	static void InCtor();
	static void InDtor();
	static void Init();
	static void Update();
	static void Render();

	static float lastFrameTime;
};