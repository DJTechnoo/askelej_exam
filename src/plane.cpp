#include "plane.h"




Plane::Plane(glm::vec3 startPos)
{
	pos = startPos;
	bank = 0.f;
	pitch = 0.0f;
}



void Plane::changePitch(float change)
{
	pitch += change;
}



void Plane::changeBank(float change)
{
	bank += change;
}