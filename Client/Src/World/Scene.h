#pragma once

class Scene final{
public:
	bool Init();
	void Update();
	void PreRender() const;
	void Render() const;
	void PostRender() const;
};