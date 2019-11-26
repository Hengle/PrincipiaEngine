#include "gameSceneSystem.h"
#include "../Utility/componentIncludes.h"
#include "../pch.h"
#include "../Physics/Components/collisionComponent.h"
#include "../Gameplay/Components/gameObjectTypeComponent.h"
#include "characterComponent.hpp"
#include "Components/enemyComponent.h"

GameSceneSystem::GameSceneSystem()
{
	addComponentType<GameSceneComponent>();
}

GameSceneSystem::~GameSceneSystem()
{
}

void GameSceneSystem::initialize()
{
	gscMapper.init(*world);
}

void GameSceneSystem::begin()
{
}

void GameSceneSystem::end()
{
}

void GameSceneSystem::added(artemis::Entity & e)
{
	int lvl = gscMapper.get(e)->levelIndex;
	e.removeComponent<GameSceneComponent>();
	//load scene
	glm::vec3 pos = glm::vec3(0.f, 1.f, 2.f);
	for (float x = -35.f; x < 36.f; x += 9.f) {
		for (float z = 15.f; z > -10.f; z -= 5.f) {
			pos = glm::vec3(x, 0.f, z);
			//artemis::Entity* enemy = SCENE.createGameShape("Enemy", pos, glm::vec3(1.f), 1, -1, true);
			//enemy->addComponent(new CollisionComponent(pos, 1));
			//enemy->addComponent(new GameObjectTypeComponent(GameObjectType::GAMEOBJECT_ENEMY));
			//enemy->addComponent(new EnemyComponent());
			//enemy->refresh();
			int m = 10 - z / 5.f;
			if (m > 12) m = 12;
			createEnemy(pos, glm::vec3(1.f), glm::vec3(2.f), m );
		}
	}
	

	pos = glm::vec3(0.f, 0.f, -15.f);
	createPlayer(pos, glm::vec3(1.f), glm::vec3(2.f));
}

void GameSceneSystem::removed(artemis::Entity & e)
{
}

void GameSceneSystem::processEntity(artemis::Entity & e)
{
}

void GameSceneSystem::createEnemy(glm::vec3 pos, glm::vec3 rot, glm::vec3 sca, int matID)
{
	artemis::Entity* enemy = createModel("Low_poly_UFO", pos, rot, sca);
	enemy->addComponent(new CollisionComponent(pos, sca.x));
	enemy->addComponent(new GameObjectTypeComponent(GameObjectType::GAMEOBJECT_ENEMY));
	enemy->addComponent(new EnemyComponent());

	NodeComponent* enemyNode = (NodeComponent*)enemy->getComponent<NodeComponent>();
	int i = 0;
	for (auto child : enemyNode->children) {
		MaterialComponent* mc = (MaterialComponent*)child->data->getComponent<MaterialComponent>();
		i == 2 ? mc->matID = 2 : mc->matID = matID;
		++i;
	}
	enemy->refresh();
	
}

void GameSceneSystem::createPlayer(glm::vec3 pos, glm::vec3 rot, glm::vec3 sca)
{
	artemis::Entity* player = createModel("Low_poly_UFO", pos, rot, sca);
	player->addComponent(new CollisionComponent(pos, sca.x));
	player->addComponent(new GameObjectTypeComponent(GameObjectType::GAMEOBJECT_PLAYER));
	player->addComponent(new CharacterComponent());
	//player->addComponent(new ControllerComponent(1));
	NodeComponent* playerNode = (NodeComponent*)player->getComponent<NodeComponent>();
	SCENE.insertController(playerNode);
	
	int i = 0;
	for (auto child : playerNode->children) {
		MaterialComponent* mc = (MaterialComponent*)child->data->getComponent<MaterialComponent>();
		//PrimitiveComponent* pc = (PrimitiveComponent*)child->data->getComponent<PrimitiveComponent>();
		i == 2 ? mc->matID = 6 : mc->matID = 2;
		//i == 2 ? pc->matId =
		child->data->refresh();
		++i;
	}
	
	player->refresh();
}

artemis::Entity * GameSceneSystem::createModel(std::string, glm::vec3 pos, glm::vec3 rot, glm::vec3 sca)
{
	rModel& resource = RESOURCEMANAGER.getModel("Low_poly_UFO");
	//Add Mesh Component and make it a parent node
	artemis::EntityManager* em = world->getEntityManager();
	artemis::Entity* entity = &em->create();
	TransformComponent* parentTransform = new TransformComponent(pos, rot, sca);
	NodeComponent* parent = new NodeComponent(entity, "enemy", COMPONENT_MODEL | COMPONENT_TRANSFORM | COMPONENT_AABB);// | COMPONENT_PRIMITIVE);

	parent->isParent = true;
	parent->isDynamic = true;
	entity->addComponent(parent);
	entity->addComponent(parentTransform);


	entity->refresh();

	//set up the subsetsx
	int i = 0;
	for (std::vector<rMesh>::const_iterator itr = resource.meshes.begin(); itr != resource.meshes.end(); itr++) {

		//Create Entity
		artemis::Entity* child = &em->create();

		//Set up subset data
		NodeComponent* childNode = new NodeComponent(child, parent);
		TransformComponent* childTransform = new TransformComponent(resource.meshes[i].center, resource.meshes[i].extents);

		child->addComponent(childNode);
		child->addComponent(childTransform);

		child->addComponent(new MeshComponent(resource.uniqueID, i));
		child->addComponent(new PrimitiveComponent(resource.uniqueID + i));
		child->addComponent(new MaterialComponent(0));
		child->addComponent(new RenderComponent(RenderType::RENDER_PRIMITIVE));
		//child->addComponent(new AABBComponent());	//SubsetAABB's point to the rendering system


		//childTransform->parentSM = &parentTransform->scaleM;
		//childTransform->parentRM = &parentTransform->rotationM;
		//childTransform->parentPM = &parentTransform->positionM;

		childNode->name = resource.meshes[i].name;// "Child " + std::to_string(i);
		childNode->flags |= COMPONENT_MESH | COMPONENT_MATERIAL | COMPONENT_AABB | COMPONENT_TRANSFORM | COMPONENT_PRIMITIVE;
		parent->children.push_back(childNode);
		//rs->addNode(childNode);

		++i;
		child->refresh();
	}
	for (i = 0; i < resource.shapes.size(); ++i) {
		//Create Entity
		artemis::Entity* child = &em->create();

		//Set up subset data
		NodeComponent* childNode = new NodeComponent(child, parent);
		TransformComponent* childTransform = new TransformComponent(resource.shapes[i].center, resource.shapes[i].extents);

		child->addComponent(childNode);
		child->addComponent(childTransform);
		child->addComponent(new PrimitiveComponent(resource.shapes[i].type));
		child->addComponent(new MaterialComponent(0));
		//child->addComponent(new AABBComponent()); //will this even be used???
		child->addComponent(new RenderComponent(RenderType::RENDER_PRIMITIVE));

		childNode->name = resource.shapes[i].name;
		childNode->flags |= COMPONENT_MATERIAL | COMPONENT_TRANSFORM | COMPONENT_PRIMITIVE;
		parent->children.push_back(childNode);
		//rs->addNode(childNode);

		child->refresh();
	}
	return entity;
}