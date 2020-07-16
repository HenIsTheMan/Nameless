#include "SpriteAni.h"

SpriteAni::SpriteAni():
	active(false),
	currFrame(0),
	rows(0),
	cols(0)
{
}

SpriteAni::SpriteAni(const int& myRows, const int& myCols):
	active(false),
	currFrame(0),
	rows(myRows),
	cols(myCols)
{
}

void SpriteAni::Play(){
	active = true;
}

void SpriteAni::Pause(){
	active = false;
}

void SpriteAni::Reset(){
	currFrame = 0;
}

void SpriteAni::Update(){
	if(active){
		++currFrame;
	}
}

void SpriteAni::Render(){
}