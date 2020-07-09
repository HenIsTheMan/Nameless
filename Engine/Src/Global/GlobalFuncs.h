#pragma once
#include "../Core.h"
#include "../Vendor/stb_image.h" //??

bool Key(const char& key);
bool InitAPI(GLFWwindow*& win);
bool InitConsole();

struct SetUpTexsParams final{
    cstr texPath;
    bool flipTex;
    int texTarget;
    int texWrapParam;
    int texFilterMin;
    int texFilterMag;
};
void SetUpTex(const SetUpTexsParams& params, uint& texRefID);

static void FramebufferSizeCallback(GLFWwindow*, int width, int height);
static void CursorPosCallback(GLFWwindow*, double xPos, double yPos);
static void MouseButtonCallback(GLFWwindow* win, int button, int action, int mods);
static void ScrollCallback(GLFWwindow*, double xOffset, double yOffset);