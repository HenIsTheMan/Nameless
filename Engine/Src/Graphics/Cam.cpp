#include "Cam.h"

#include "../Global/GlobalFuncs.h"
#include "../Global/GlobalVars.h"

///Not to be used
Cam::Cam():
	Cam(
		glm::vec3(0.0f),
		glm::vec3(0.0f),
		0.0f,
		0.0f
	)
{
	assert(false);
}

Cam::Cam(const glm::vec3& pos, const glm::vec3& target, const float aspectRatio, const float spd):
	aspectRatio(aspectRatio),
	spd(spd),
	pos(pos),
	target(target),
	defaultAspectRatio(aspectRatio),
	defaultSpd(spd),
	defaultPos(pos),
	defaultTarget(target)
{
}

glm::vec3 Cam::CalcFront() const{
	return glm::normalize(target - pos);
}

glm::vec3 Cam::CalcRight() const{
	return glm::cross(CalcFront(), glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::vec3 Cam::CalcUp() const{
	return glm::cross(CalcFront(), CalcRight());
}

glm::mat4 Cam::LookAt() const{
	return glm::lookAt(pos, target, glm::vec3(0.0f, 1.0f, 0.0f));
}

void Cam::Update(
	const float dt,
	const int up,
	const int down,
	const int left,
	const int right,
	const int front,
	const int back
){
	float upDown = float(Key(up) - Key(down));
	float leftRight = float(Key(left) - Key(right));
	float frontBack = float(Key(front) - Key(back));

	const glm::vec3 camFront = CalcFront();
	glm::vec3&& xzCamFront = glm::vec3(camFront.x, 0.f, camFront.z);
	if(xzCamFront != glm::vec3(0.f)){
		xzCamFront = glm::normalize(xzCamFront);
	}
	if(glm::dot(glm::vec3(0.0f, 0.0f, -1.0f), camFront) < 0.0f){
		xzCamFront = -xzCamFront;
	}

	glm::vec3&& change = frontBack * xzCamFront + glm::vec3(0.f, upDown, 0.f) + leftRight * -CalcRight() + float(LMB - RMB) * camFront;
	if(change != glm::vec3(0.f)){
		change = normalize(change);
	}
	pos += spd * dt * change;
	target = pos + camFront;

	glm::highp_vec3 rotatedCamFront = glm::rotate(glm::angleAxis(-glm::radians(pitch), CalcRight()), camFront);
	rotatedCamFront = glm::rotate(glm::angleAxis(glm::radians(yaw), CalcUp()), rotatedCamFront);

	target = pos + rotatedCamFront;

	yaw = 0.0f;
	pitch = 0.0f;
}

void Cam::Reset(){
	aspectRatio = defaultAspectRatio;
	spd = defaultSpd;
	pos = defaultPos;
	target = defaultTarget;
}

void Cam::ResetAspectRatio(){
	aspectRatio = defaultAspectRatio;
}

void Cam::ResetSpd(){
	spd = defaultSpd;
}

void Cam::ResetPos(){
	pos = defaultPos;
}

void Cam::ResetTarget(){
	target = defaultTarget;
}

float Cam::GetAspectRatio() const{
	return aspectRatio;
}

float Cam::GetSpd() const{
	return spd;
}

const glm::vec3& Cam::GetPos() const{
	return pos;
}

const glm::vec3& Cam::GetTarget() const{
	return target;
}

float Cam::GetDefaultAspectRatio() const{
	return defaultAspectRatio;
}

float Cam::GetDefaultSpd() const{
	return defaultSpd;
}

const glm::vec3& Cam::GetDefaultPos() const{
	return defaultPos;
}

const glm::vec3& Cam::GetDefaultTarget() const{
	return defaultTarget;
}

void Cam::SetAspectRatio(const float aspectRatio){
	this->aspectRatio = aspectRatio;
}

void Cam::SetSpd(const float spd){
	this->spd = spd;
}

void Cam::SetPos(const glm::vec3& pos){
	this->pos = pos;
}

void Cam::SetTarget(const glm::vec3& target){
	this->target = target;
}

void Cam::SetDefaultAspectRatio(const float defaultAspectRatio){
	this->defaultAspectRatio = defaultAspectRatio;
}

void Cam::SetDefaultSpd(const float defaultSpd){
	this->defaultSpd = defaultSpd;
}

void Cam::SetDefaultPos(const glm::vec3& defaultPos){
	this->defaultPos = defaultPos;
}

void Cam::SetDefaultTarget(const glm::vec3& defaultTarget){
	this->defaultTarget = defaultTarget;
}