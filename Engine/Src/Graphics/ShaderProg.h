#pragma once
#include "../Core.h"

class ShaderProg final{
	static ShaderProg* currShaderProg;
	cstr shaderPaths[3];
	static uint texRefIDs[32];
	std::unordered_map<str, int> uniLocationCache;
	static std::unordered_map<cstr, uint> shaderCache;

	uint refID;
	int GetUniLocation(cstr const& uniName);
	void ParseShader(cstr const& fPath, const uint& shaderRefID) const;
	void Link() const;
public:
	ShaderProg();
	ShaderProg(cstr const& vsPath, cstr const& fsPath, cstr const& gsPath = "");
	~ShaderProg();
	void Use();

	///Utility funcs
	void Set1f(cstr const& uniName, const float& val);
	void Set3f(cstr const& uniName, const float& a, const float& b, const float& c);
	void Set3fv(cstr const& uniName, const glm::vec3& vec);
	void Set1i(cstr const& uniName, const int& val);
	void SetMat4fv(cstr const& uniName, const float* const& floatPtr, const bool& transpose = false);
};