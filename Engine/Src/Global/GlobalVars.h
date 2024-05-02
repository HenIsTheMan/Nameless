#pragma once

bool endLoop = false;
int myArgc;
const char* const* myArgv;

bool LMB = false;
bool RMB = false;
float pitch = 0.0f;
float yaw = 0.0f;
double lastX = 0.0;
double lastY = 0.0;
float SENS = 0.05f;
float angularFOV = 45.0f;
int optimalWinXPos;
int optimalWinYPos;
int optimalWinWidth;
int optimalWinHeight;
int winWidth;
int winHeight;