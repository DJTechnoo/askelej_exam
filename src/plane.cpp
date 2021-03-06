#include "plane.h"




Plane::Plane(glm::vec3 startPos)
{
	pos = startPos;
	bank = 0.f;
	pitch = 0.0f;
	airSpeed =0.1f;
}



void Plane::changePitch(float change)
{
	pitch += change;
}



void Plane::changeBank(float change)
{
	bank += change;
}


void Plane::changeAirSpeed(float change)
{
	airSpeed += change;
	if (airSpeed < 0.1f) airSpeed = 0.1f;
}