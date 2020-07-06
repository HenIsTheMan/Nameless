#pragma once
#include <random>

template <class T>
inline T PseudorandMinMax(const T& min, const T& max){
	return min + T(rand()) / T(RAND_MAX / (max - min));
}

template <>
inline int PseudorandMinMax<int>(const int& min, const int& max){
	const int&& absMin = abs(min);
	return rand() % (max + abs(min) + 1) - abs(min);
}