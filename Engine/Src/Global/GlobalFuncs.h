#pragma once
#include "GlobalStructs.h"

bool Key(const char& key);
bool Key(const int& key);
bool InitAPI(GLFWwindow*& win);
bool InitConsole();

void SetUpTex(const SetUpTexsParams& params, uint& texRefID);

template <class T>
inline T& operator++(T& myEnum){
	myEnum = T((int)myEnum + 1);
	return myEnum;
}

static void FramebufferSizeCallback(GLFWwindow*, int width, int height);
static void CursorPosCallback(GLFWwindow*, double xPos, double yPos);
static void MouseButtonCallback(GLFWwindow* win, int button, int action, int mods);
static void ScrollCallback(GLFWwindow*, double xOffset, double yOffset);