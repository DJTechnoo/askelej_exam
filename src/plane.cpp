#include "plane.h"




Plane::Plane(glm::vec3 startPos)
{
	pos = startPos;
	bank = 90.f;
	pitch = 90.0f;
}



void Plane::changePitch(float change)
{
	pitch += change;
}



void Plane::changeBank(float change)
{
	bank += change;
}