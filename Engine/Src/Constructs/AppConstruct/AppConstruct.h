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
	template <class T>
	static void SetInCtor(void (T::*inCtor)());

	template <class T>
	static void SetInDtor(void (T::*inDtor)());

	template <class T>
	static void SetInit(void (T::*init)());

	template <class T>
	static void SetUpdate(void (T::*update)(const float dt));

	template <class T>
	static void SetRender(void (T::*render)());
private:
	template <class T>
	static void (T::*sm_InCtor)();

	template <class T>
	static void (T::*sm_InDtor)();

	template <class T>
	static void (T::*sm_Init)();

	template <class T>
	static void (T::*sm_Update)(const float dt);

	template <class T>
	static void (T::*sm_Render)();

	static void InCtor();

	static void InDtor();

	static void Init();

	static void Update();

	static void Render();

	static float lastFrameTime;
};

#include "AppConstruct.inl"