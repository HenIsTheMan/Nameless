#pragma once

class YesScene final{ //Static class
	YesScene() = delete;
	~YesScene() = delete;
	YesScene(const YesScene& yesScene) = default;
	YesScene(YesScene&& yesScene) noexcept = default;
	YesScene& operator=(const YesScene& yesScene) = default;
	YesScene& operator=(YesScene&& yesScene) noexcept = default;
public:
	static void Init();
	static void FixedUpdate(float dt);
	static void Update(float dt);
	static void LateUpdate(float dt);
	static void PreRender();
	static void Render();
	static void PostRender();
};