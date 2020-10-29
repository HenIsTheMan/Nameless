#include "Cam.h"

#include "../../../../../Global/GlobalFuncs.h"
#include "../../../../../Global/GlobalVars.h"

static glm::quat QuatBetweenVecs(const glm::vec3& src, const glm::vec3& dst){
	const glm::vec3 srcNormalised = glm::normalize(src);
	const glm::vec3 dstNormalised = glm::normalize(dst);
	const float cosTheta = glm::dot(srcNormalised, dstNormalised);
	glm::vec3 axis;

	if(cosTheta < -1.0f + glm::epsilon<float>()){ //If src and dst are in opp dirs (no ideal axis so guess 1, curr implementation favours rotation arnd y-axis)...
		axis = glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), src);
		if(glm::dot(axis, axis) < glm::epsilon<float>()){ //If src and vec3(0, 0, 1) are parallel...
			axis = glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), src);
		}
		return glm::angleAxis(180.0f, glm::normalize(axis));
	}

	axis = glm::cross(src, dst);
	const float qty = (float)sqrt((1.0f + cosTheta) * 2.0f);
	return glm::quat(qty * 0.5f, axis / qty);
}

Cam::Cam():
	aspectRatio(0.f),
	spd(0.f),
	pos(glm::vec3(0.f)),
	target(glm::vec3(0.f)),
	defaultAspectRatio(0.f),
	defaultSpd(0.f),
	defaultPos(glm::vec3(0.f)),
	defaultTarget(glm::vec3(0.f))
{
}

Cam::Cam(const glm::vec3& pos, const glm::vec3& target, const float& aspectRatio, const float& spd):
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

glm::vec3 Cam::CalcFront(const bool& normalised) const{
	const glm::vec3 camFront = target - pos;
	return normalised && camFront != glm::vec3(0.f) ? glm::normalize(camFront) : camFront;
}

glm::vec3 Cam::CalcRight() const{
	const glm::vec3 camRight = QuatBetweenVecs(glm::vec3(0.0f, 0.0f, -1.0f), CalcFront()) * glm::vec3(1.0f, 0.0f, 0.0f);
	return camRight != glm::vec3(0.f) ? glm::normalize(camRight) : camRight;
}

glm::vec3 Cam::CalcUp() const{
	const glm::vec3 camUp = QuatBetweenVecs(glm::vec3(0.0f, 0.0f, -1.0f), CalcFront()) * glm::vec3(0.0f, 1.0f, 0.0f);
	return camUp != glm::vec3(0.f) ? glm::normalize(camUp) : camUp;
}

glm::mat4 Cam::LookAt() const{
	glm::vec3 vecArr[]{CalcRight(), CalcUp(), -CalcFront()};
	glm::mat4 translation = glm::mat4(1.f), rotation = glm::mat4(1.f);
	for(short i = 0; i < 3; ++i){ //Access elements as mat[col][row] due to column-major order
		translation[3][i] = -pos[i];
		for(short j = 0; j < 3; ++j){
			rotation[i][j] = (vecArr[j])[i];
		}
	}
	return rotation * translation;
}

void Cam::Update(float dt, const int& up, const int& down, const int& left, const int& right, const int& front, const int& back){
	const float camSpd = spd * dt;
	float upDown = float(Key(up) - Key(down));
	float leftRight = float(Key(left) - Key(right));
	float frontBack = float(Key(front) - Key(back));

	const glm::vec3 camFront = CalcFront();
	glm::vec3&& xzCamFront = glm::vec3(camFront.x, 0.f, camFront.z);
	if(xzCamFront != glm::vec3(0.f)){
		xzCamFront = glm::normalize(xzCamFront);
	}

	glm::vec3 change = frontBack * xzCamFront + glm::vec3(0.f, upDown, 0.f) + leftRight * -CalcRight() + float(LMB - RMB) * camFront;
	if(change != glm::vec3(0.f)){
		change = normalize(change);
	}
	pos += camSpd * change;
	target = pos + camFront;

	glm::quat pitchQuat = glm::angleAxis(glm::radians(yaw), CalcUp());
	glm::quat yawQuat = glm::angleAxis(glm::radians(pitch), CalcRight());
	glm::quat pitchYaw = glm::normalize(glm::slerp(pitchQuat, yawQuat, 0.5f));
	target = pos + pitchYaw * camFront;
	yaw = pitch = 0.f;
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

const float& Cam::GetAspectRatio() const{
	return aspectRatio;
}

const float& Cam::GetSpd() const{
	return spd;
}

const glm::vec3& Cam::GetPos() const{
	return pos;
}

const glm::vec3& Cam::GetTarget() const{
	return target;
}

const float& Cam::GetDefaultAspectRatio() const{
	return defaultAspectRatio;
}

const float& Cam::GetDefaultSpd() const{
	return defaultSpd;
}

const glm::vec3& Cam::GetDefaultPos() const{
	return defaultPos;
}

const glm::vec3& Cam::GetDefaultTarget() const{
	return defaultTarget;
}

void Cam::SetAspectRatio(const float& aspectRatio){
	this->aspectRatio = aspectRatio;
}

void Cam::SetSpd(const float& spd){
	this->spd = spd;
}

void Cam::SetPos(const glm::vec3& pos){
	this->pos = pos;
}

void Cam::SetTarget(const glm::vec3& target){
	this->target = target;
}

void Cam::SetDefaultAspectRatio(const float& defaultAspectRatio){
	this->defaultAspectRatio = defaultAspectRatio;
}

void Cam::SetDefaultSpd(const float& defaultSpd){
	this->defaultSpd = defaultSpd;
}

void Cam::SetDefaultPos(const glm::vec3& defaultPos){
	this->defaultPos = defaultPos;
}

void Cam::SetDefaultTarget(const glm::vec3& defaultTarget){
	this->defaultTarget = defaultTarget;
}