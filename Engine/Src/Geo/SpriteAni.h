#pragma once
#include "Mesh.h"

class SpriteAni final: public Mesh{
public:
	SpriteAni();
	SpriteAni(const int& myRows, const int& myCols);
	~SpriteAni() = default;

	void Play();
	void Pause();
	void Reset();
	void Update();
	void Render();
private:
	bool active;
	int currFrame;
	int rows;
	int cols;
};