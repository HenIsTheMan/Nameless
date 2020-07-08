#pragma once

#include <ASSIMP/Importer.hpp>
#include <ASSIMP/scene.h>
#include <ASSIMP/postprocess.h>
#include <Windows.h>
#include <GLAD/glad.h>
#include <GLFW/glfw3.h>
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>
#include <IRRKLANG/irrKlang.h>

#include <fstream>
#include <thread>
#include <unordered_map>
#include <vector>

using namespace irrklang;

constexpr auto KEY_2 = 50;

#define STR(text) #text

#pragma comment(lib, "assimp-vc142-mtd.lib")
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "irrKlang.lib")
#pragma comment(lib, "opengl32.lib")

typedef const char* cstr;
typedef unsigned int uint;
typedef std::string str;