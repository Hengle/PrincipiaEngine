#include "collisionSystem.h"
#include "collisionTests.h"
#include "../Utility/helpers.h"

namespace Principia {
	CollisionSystem::CollisionSystem()
	{
		addComponentType<CollisionComponent>();
		addComponentType<TransformComponent>();
		addComponentType<DynamicComponent>();
	}

	CollisionSystem::~CollisionSystem()
	{
	}

	void CollisionSystem::initialize()
	{
		colMapper.init(*world);
		transMapper.init(*world);
		grid = (GridComponent*)world->getSingleton()->getComponent<GridComponent>();
	}

	void CollisionSystem::begin()
	{
		checkDynamicCollisions();
	}


	void CollisionSystem::processEntity(artemis::Entity & e)
	{
		//Goes through every object and checks for collision
		TransformComponent* tc = transMapper.get(e);
		CollisionComponent* col = colMapper.get(e);
		col->position = tc->world[3];
		//col->extents = rotateBounds(tc->global.rotation, col->extents);

		//This is for all static objects ///////////////do later right now u finna test spheres
		checkStaticCollision(e);		



	}

	void CollisionSystem::checkDynamicCollisions()
	{
		auto* actives = getActives();
		int c = actives->getCount();

		for (int i = 0; i < c - 1; ++i) {
			auto a = actives->get(i);
			for (int j = i + 1; j < c; ++j) {
				auto b = actives->get(j);
				checkCollision(*a, *b);
			}
		}
	}

	void CollisionSystem::checkStaticCollision(artemis::Entity & e)
	{
		std::vector<artemis::Entity*> collisions;
		CollisionComponent* cc = colMapper.get(e);
		GridBlock gb = GridBlock(cc->position, cc->extents);
		if (!gb.verify(grid->size))
			return;
		for (int r = gb.leftx; r < gb.rightx; r += gb.itr) {
			for (int c = gb.downy; c < gb.upy; c += gb.itr) {
				if (grid->grid[r][c] != nullptr) {
					if (std::find(collisions.begin(), collisions.end(), grid->grid[r][c]) == collisions.end())
						collisions.push_back(grid->grid[r][c]);
				}
			}
		}

		//check collision list for collisions
		if (collisions.size() > 0) {
			for (auto& c : collisions)
				checkCollision(e, *c);
		}
	}

	void CollisionSystem::checkCollision(artemis::Entity & a, artemis::Entity & b)// const
	{
		CollisionComponent* ccA = colMapper.get(a);
		CollisionComponent* ccB = colMapper.get(b);
		CollisionData cda = CollisionData(b.getId());
		CollisionData cdb = CollisionData(a.getId());
		
		if (CollisionTests::ColTests[CollisionTests::Convert(ccA->type, ccB->type)](ccA, ccB, cda, cdb)) {
			CollidedComponent* cwa = (CollidedComponent*)a.getComponent<CollidedComponent>();
			CollidedComponent* cwb = (CollidedComponent*)b.getComponent<CollidedComponent>();

			
			cwa == nullptr ?
				a.addComponent(new CollidedComponent(cda))
				:
				cwa->update(cda);

			cwb == nullptr ?
				b.addComponent(new CollidedComponent(cdb))
				:
				cwb->update(cdb);

			a.refresh();
			b.refresh();
		}
	}

	glm::vec3 CollisionSystem::rotateBounds(const glm::quat & m, const glm::vec3 & extents)
	{
		//set up cube
		glm::vec3 v[8];
		v[0] = extents;
		v[1] = glm::vec3(extents.x, extents.y, -extents.z);
		v[2] = glm::vec3(extents.x, -extents.y, -extents.z);
		v[3] = glm::vec3(extents.x, -extents.y, extents.z);
		v[4] = glm::vec3(-extents);
		v[5] = glm::vec3(-extents.x, -extents.y, extents.z);
		v[6] = glm::vec3(-extents.x, extents.y, -extents.z);
		v[7] = glm::vec3(-extents.x, extents.y, extents.z);

		//transform them
#pragma omp parallel for
		for (int i = 0; i < 8; ++i) {
			v[i] = abs(m * v[i]);// glm::vec4(v[i], 1.f));

		}

		//compare them
		glm::vec3 vmax = glm::vec3(FLT_MIN);
		for (int i = 0; i < 8; ++i) {
			vmax.x = tulip::max(vmax.x, v[i].x);
			vmax.y = tulip::max(vmax.y, v[i].y);
			vmax.z = tulip::max(vmax.z, v[i].z);
		}

		return vmax;
	}
}