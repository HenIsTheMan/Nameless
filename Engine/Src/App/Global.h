#pragma once

inline bool firstCall = 1;
inline float dt = 0.f;
inline float leftRightMB = 0.f;
inline float pitch = 0.f;
inline float yaw = 0.f;
inline float lastX = 0.f;
inline float lastY = 0.f;
inline float SENS = .05f;
inline float angularFOV = 45.f;

static inline const bool Key(const char& key){
	return GetAsyncKeyState((unsigned short)key) & 0x8000;
}