#pragma once
#include <Core.h>

class Cam final{
	float aspectRatio;
	float spd;
	glm::vec3 pos;
	glm::vec3 target;
	glm::vec3 up;

	float defaultAspectRatio;
	float defaultSpd;
	glm::vec3 defaultPos;
	glm::vec3 defaultTarget;
	glm::vec3 defaultUp;
public:
	enum struct CamAttrib{
		AspectRatio = 0,
		Spd = 1,
		Pos = 2,
		Target = 3,
		Up = 4,
		DefaultAspectRatio = 5,
		DefaultSpd = 6,
		DefaultPos = 7,
		DefaultTarget = 8,
		DefaultUp = 9,
	};

	Cam();
	Cam(const glm::vec3& myPos, const glm::vec3& myTarget, const glm::vec3& myUp, const float& myAspectRatio, const float& mySpd);
	~Cam() = default;
	glm::vec3 CalcFront(const bool& normalised) const;
	glm::vec3 CalcRight() const;
	glm::vec3 CalcUp() const;
	glm::mat4 LookAt() const;
	void Update(const int& up, const int& down, const int& left, const int& right, const int& front, const int& back);
	void Reset();

	inline const std::variant<float, glm::vec3>& Get(const CamAttrib& attrib) const;

	///Setters
	//void SetPos(const glm::vec3& pos);
	//void SetTarget(const glm::vec3& target);
	//void SetUp(const glm::vec3& up);
	//void SetDefaultPos(const glm::vec3& defaultPos);
	//void SetDefaultTarget(const glm::vec3& defaultTarget));
	//void SetDefaultUp(const glm::vec3& defaultUp);





};