#pragma once

#include "GlobalStructs.h"

bool Key(const char& key);
bool Key(const int& key);
bool InitAPI(GLFWwindow*& window);
bool InitConsole();

void SetUpTex(const SetUpTexsParams& params, uint& texRefID);

template <class T>
inline T& operator++(T& myEnum){
	myEnum = T((int)myEnum + 1);
	return myEnum;
}

//* Forward declarations
static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
static void InitialCursorPosCallback(GLFWwindow* window, double xPos, double yPos);
static void CursorPosCallback(GLFWwindow* window, double xPos, double yPos);
static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
static void ScrollCallback(GLFWwindow* window, double xOffset, double yOffset);
//*/