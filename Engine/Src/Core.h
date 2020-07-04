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
#include "Vendor/stb_image.h"

#include <fstream>
#include <thread>
#include <unordered_map>
#include <variant>
#include <vector>

typedef const char* cstr;
typedef unsigned int uint;
typedef std::string str;