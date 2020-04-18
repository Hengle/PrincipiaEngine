#pragma once
#include "Components/collisionComponent.h"
#include "Components/dynamicComponent.h"
#include "Components/gridComponent.h"
#include "Artemis/EntityProcessingSystem.h"
#include "Artemis/ComponentMapper.h"
#include "Artemis/Entity.h"
//#include "../Game/Gameplay/Components/gameObjectTypeComponent.h"
#include "../Utility/transformComponent.hpp"
#include <set>

namespace Principia {
	class CollisionSystem : public artemis::EntityProcessingSystem {
	public:
		CollisionSystem();
		~CollisionSystem();

		void initialize();
		void begin();
		void processEntity(artemis::Entity &e) override;

		void checkDynamicCollisions();
		void checkStaticCollision(artemis::Entity& e);
		void checkCollision(artemis::Entity& a, artemis::Entity& b);
		glm::vec3 rotateBounds(const glm::quat & m, const glm::vec3 & extents);

	private:
		artemis::ComponentMapper<CollisionComponent> colMapper;
		artemis::ComponentMapper<TransformComponent> transMapper;
		artemis::ComponentMapper<DynamicComponent> dynamicMapper;
		//artemis::ComponentMapper<GameObjectTypeComponent> gotMapper;

		GridComponent* grid;

	};
}