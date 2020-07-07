#include "ShaderProg.h"

ShaderProg* ShaderProg::currShaderProg = nullptr;
uint ShaderProg::texRefIDs[32];
std::unordered_map<cstr, uint> ShaderProg::shaderCache;

ShaderProg::ShaderProg():
	refID(0),
	shaderPaths{"", "", ""}
{
}

ShaderProg::ShaderProg(cstr const& vsPath, cstr const& fsPath, cstr const& gsPath):
	refID(0),
	shaderPaths{vsPath, fsPath, gsPath}
{
}

ShaderProg::~ShaderProg(){
	if(!shaderCache.empty()){
		for(const auto& shader: shaderCache){
			glDeleteShader(shader.second);
		}
		shaderCache.clear();
	}
	glDeleteProgram(refID);
}

int ShaderProg::GetUniLocation(cstr const& uniName){
	if(!uniLocationCache.count(str{uniName})){ //If not cached...
		uniLocationCache[str{uniName}] = glGetUniformLocation(refID, uniName); //Query location of uni
		if(uniLocationCache[str{uniName}] == -1){
			printf("%u: Failed to find uni '%s'\n", this->refID, uniName);
		}
	}
	return uniLocationCache[str{uniName}];
}

bool ShaderProg::Init(){
	if(shaderPaths[0] == ""){
		return false;
	}
	refID = glCreateProgram();
	for(short i = 0; i < sizeof(shaderPaths) / sizeof(shaderPaths[0]) - (shaderPaths[2] == ""); ++i){
		if(shaderCache.count(shaderPaths[i])){
			printf("Reusing \"%s\"...\n", shaderPaths[i]);
			glAttachShader(refID, shaderCache[shaderPaths[i]]);
		} else{
			uint shaderRefID = glCreateShader(i < 2 ? (~i & 1 ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER) : GL_GEOMETRY_SHADER);
			ParseShader(shaderPaths[i], shaderRefID);
			glAttachShader(refID, shaderRefID);
			shaderCache[shaderPaths[i]] = shaderRefID;
		}
	}
	Link();
	return true;
}

void ShaderProg::Link() const{
	int infoLogLength;
	puts("Linking prog...\n");
	glLinkProgram(refID); //Vars in diff shaders are linked here too

	glGetProgramiv(refID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if(infoLogLength){
		char* errorMsg = (char*)_malloca(infoLogLength * sizeof(char)); //Allocate memory on the stack dynamically
		glGetProgramInfoLog(refID, infoLogLength, &infoLogLength, errorMsg);
		printf("%s\n", errorMsg);
	}
}

void ShaderProg::ParseShader(cstr const& fPath, const uint& shaderRefID) const{
	int infoLogLength;
	str srcCodeStr, line;
	std::ifstream stream(fPath);

	if(!stream.is_open()){
		printf("Failed to open and read \"%s\"\n", fPath);
		return;
	}
	while(getline(stream, line)){
		srcCodeStr += "\n" + line;
	}
	stream.close();

	printf("Compiling \"%s\"...\n", fPath);
	cstr srcCodeCStr = srcCodeStr.c_str();
	glShaderSource(shaderRefID, 1, &srcCodeCStr, 0);
	glCompileShader(shaderRefID);
	glGetShaderiv(shaderRefID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if(infoLogLength){
		char* errorMsg = (char*)_malloca(infoLogLength * sizeof(char)); //Allocate memory on the stack dynamically
		glGetShaderInfoLog(shaderRefID, infoLogLength, &infoLogLength, errorMsg);
		printf("Failed to compile \"%s\"!\n%s\n", fPath, errorMsg);
	}
}

void ShaderProg::Use(){
	if(!refID && !Init()){ //Init on 1st use
		printf("%u: ShaderProg not initialised\n", this->refID);
	}
	if(!currShaderProg || currShaderProg->refID != refID){
		glUseProgram(refID);
		currShaderProg = this;
	}
}

void ShaderProg::Set1f(cstr const& uniName, const float& val){
	glUniform1f(GetUniLocation(uniName), val);
}

void ShaderProg::Set3f(cstr const& uniName, const float& a, const float& b, const float& c){
	glUniform3f(GetUniLocation(uniName), a, b, c);
}

void ShaderProg::Set3fv(cstr const& uniName, const glm::vec3& vec){
	glUniform3fv(GetUniLocation(uniName), 3, &vec[0]);
}

void ShaderProg::Set1i(cstr const& uniName, const int& val){
	glUniform1i(GetUniLocation(uniName), val);
}

void ShaderProg::SetMat4fv(cstr const& uniName, const float* const& floatPtr, const bool& transpose){
	glUniformMatrix4fv(GetUniLocation(uniName), 1, transpose, floatPtr);
}