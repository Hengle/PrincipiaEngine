#ifndef  TRANSFORMCOMPONENT_HPP
#define TRANSFOMRCOMPONENT_HPP

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>
#include "helper.h"

struct Bounds {
	glm::vec3 center;
	glm::vec3 extents;

	Bounds(const glm::vec3& c, const glm::vec3& e) : center(c), extents(e) {};
	Bounds() { center = glm::vec3(0); extents = glm::vec3(0); };
	inline glm::vec3 max() {
		return center + extents;
	}
	inline glm::vec3 min() {
		return center - extents;
	}

	Bounds combine(Bounds& b) {
		//find the highest and the lowest x and y values
		glm::vec3 max = maxV(this->max(), b.max());
		glm::vec3 min = minV(this->min(), b.min());

		//center = halfway between the two, extents = max-center
		glm::vec3 c = (max + min) * 0.5f;
		glm::vec3 e = max - c;

		return Bounds(c, e);
	}

	float Offset(glm::vec3 c, int a) const {
		float ret =  (c[a] - (center[a] - extents[a])) / (extents[a] * 2);
		return ret;
	}

	float SurfaceArea() {
		glm::vec3 te = extents * 2.f;
		return 2 * (te.x * te.y + te.x * te.z + te.y * te.z);
	}
};

struct Bucket {
	int count = 0;
	Bounds bounds;
};

struct sqt {
	glm::quat rotation;
	glm::vec3 position;
	glm::vec3 scale;

private:
	float _pad[2];
}; //48bytes //can be 40 if necessary

struct TransformComponent{
	glm::mat4 world; //	translation rotation scale matrix
	glm::mat4 TRM;  // note: this is a Translation+Rotation Matrix, scale is left separate
	sqt local;
	sqt global;

	glm::vec3 eulerRotation;

	TransformComponent() { local.scale = glm::vec3(1.f); local.rotation = glm::quat(); local.position = glm::vec3(0); world = glm::mat4(0); };
	TransformComponent(glm::vec3 center, glm::vec3 extents) {
		*this = TransformComponent(center, glm::vec3(0.f), extents);
	}
	TransformComponent(glm::vec3 pos, glm::vec3 rot, glm::vec3 sca) : eulerRotation(rot) {

		
		glm::mat4 rotationM;
		rotationM = glm::rotate(rotationM, glm::radians(rot.x), glm::vec3(1.0f, 0.0f, 0.0f));
		rotationM = glm::rotate(rotationM, glm::radians(rot.y), glm::vec3(0.0f, 1.0f, 0.0f));
		rotationM = glm::rotate(rotationM, glm::radians(rot.z), glm::vec3(0.0f, 0.0f, 1.0f));
		local.rotation = glm::toQuat(rotationM);
		local.position = pos;
		local.scale = sca;

		world = rotationM;
		world[3] = glm::vec4(pos, 1.f);
		//global.rotation = local.rotation;
		//global.position = local.position;
		global.scale = local.scale;
	};

	inline Bounds toBounds() { return Bounds(glm::vec3(world[3]), global.scale); };
};

#endif // ! TRANSFORMCOMPONENT_HPP

