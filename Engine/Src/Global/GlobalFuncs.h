#pragma once
#include "../Core.h"

bool Key(const char& key);
bool InitAPI(GLFWwindow*& win);
static void FramebufferSizeCallback(GLFWwindow*, int width, int height);
static void CursorPosCallback(GLFWwindow*, double xPos, double yPos);
static void MouseButtonCallback(GLFWwindow* win, int button, int action, int mods);
static void ScrollCallback(GLFWwindow*, double xOffset, double yOffset);