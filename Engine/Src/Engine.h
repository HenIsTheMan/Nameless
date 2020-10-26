#pragma once

#include "Geo/Mesh.h"
#include "Geo/Model.h"
#include "Geo/SpriteAni.h"
#include "Geo/Terrain.h"
#include "Global/GlobalFuncs.h"
#include "Graphics/Light.h"
#include "Graphics/ShaderProg.h"
#include "Graphics/TextChief.h"

#include <random>

template <class T>
inline T PseudorandMinMax(const T& min, const T& max){
	std::random_device rd;
	std::mt19937 generator(rd());
	std::uniform_real_distribution<T> distribution(min, max);
	return distribution(generator);
}

template <>
inline int PseudorandMinMax<int>(const int& min, const int& max){
	std::random_device rd;
	std::mt19937 generator(rd());
	std::uniform_int_distribution<int> distribution(min, max);
	return distribution(generator);
}