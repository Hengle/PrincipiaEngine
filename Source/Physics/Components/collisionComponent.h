#pragma once
#include "Artemis/Component.h"
#include <glm/glm.hpp>
#include <unordered_map>

namespace Principia {
	enum class CollisionState {
		None, Enter, Continue, Exit
	};
	enum class CollisionType {
		Sphere = 0x00, 
		Box = 0x01, 
		Capsule = 0x02,
		Other = 0x03,
	};
	
	//This component is passed to any entity you wish to have collisions with
	struct CollisionComponent : artemis::Component {
		bool checked = false;
		int id;
		CollisionType type;
		glm::vec3 extents;
		glm::vec3 position;
		CollisionComponent(glm::vec3 pos, glm::vec3 e, CollisionType t) : position(pos), extents(e), type(t) {}
	};

	//When you collide with an entity, this data is passed to it
	struct CollisionData{
		int id;
		int timer = 0;
		CollisionState state = CollisionState::None;
		glm::vec3 position;
		CollisionData() {};
		CollisionData(int i, glm::vec3 p) : id(i), position(p) {};
	};

	//This component is passed to an entity when it collides with it
	struct CollidedComponent : artemis::Component {
		std::vector<CollisionData> collidedWith;
		CollidedComponent(CollisionData d) {
			collidedWith.push_back(d);
		}
		void update(CollisionData cd) {
			bool found = false;
			for (auto& cw : collidedWith) {
				if (cw.id == cd.id) {
					cw.position = cd.position;
					cw.timer++;
				}
			}
			if (!found)
				collidedWith.push_back(cd);
		}
	};


}