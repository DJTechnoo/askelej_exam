#ifndef __PLANE_H
#define __PLANE_H

#include "glm/glm/glm.hpp"
#include "glm/glm/gtc/matrix_transform.hpp"
#include "glm/glm/gtc/type_ptr.hpp"

class Plane {
public:
	float pitch, bank, airSpeed;
	glm::vec3 pos;
	Plane(glm::vec3 startPos);
	void changePitch(float change);
	void changeBank(float change);
	void changeAirSpeed(float change);


};


#endif