#pragma once
/*
Scene Copyright (C) by Mike Murrell 
this is where all the scene building goes which means
loading/saving scene, creating/copying/deleting objects
*/


#include "../pch.h"
#include "../Utility/resourceManager.h"
#include "../Animation/animationSystem.h"
#include "script.hpp"
#include "transformSystem.h"
#include "Application/controllerSystem.h"
#include "Gameplay/characterController.h"
#include "../Audio/audioComponents.h"
#include "../Utility/bvhSystem.h"

#define SCENE Scene::get()


class Scene{ //: public artemis::EntityProcessingSystem {
private:
	Scene() {};
public:
	~Scene() {};
	static Scene& get() {
		static Scene instance;
		return instance;
	}
	Scene(Scene const&) = delete;
	void operator=(Scene const&) = delete;

	artemis::World* world;
	artemis::EntityManager* em;
	artemis::SystemManager* sm;

	RenderSystem*	rs;
	TransformSystem* ts;
	EngineUISystem* ui;
	AnimationSystem* as;

	CharacterController* cc;
	ControllerSystem* input;

	std::vector<NodeComponent*> parents;
	std::vector<TransformComponent*> transforms;

	BvhSystem* bvh;

	int sceneNumber;
	
	void init(artemis::World& w);

	void doStuff();
	void postStart();
	void buildBVH();

	void createModel(rModel resource, std::string name, glm::vec3 pos, glm::vec3 rot, glm::vec3 sca, bool dynamic = true);
	void createSkinnedModel(rSkeleton resource, std::string name, glm::vec3 pos, glm::vec3 rot, glm::vec3 sca, bool dynamic = true);

	artemis::Entity* createShape(std::string name, glm::vec3 pos, glm::vec3 scale, int matID, int type, bool dynamic = true);
	artemis::Entity* createGameShape(std::string name, glm::vec3 pos, glm::vec3 scale, int matID, int type, bool dynamic = true);

	void insertController(NodeComponent* nc);
	void insertRigidBody(NodeComponent* nc);
	void insertGoal(artemis::Entity& e);
	artemis::Entity* createLight();

	artemis::Entity* createCamera();

	void deleteNode(std::vector<NodeComponent*>& nParents, int nIndex);
	void deleteNode(NodeComponent* parent);
	void copyNode(NodeComponent* node, NodeComponent* parent, std::vector<NodeComponent*>& list);
	void makeParent(NodeComponent * child);
	void makeChild(NodeComponent* node, NodeComponent* parent, std::vector<NodeComponent*>& list);

	void updateObject(NodeComponent* node);
	//void addNode(ComponentFlag flags, std::string name, TransformComponent tc, int matID = 0, int meshID = 0);

	tinyxml2::XMLError SaveScene(std::string name);
	tinyxml2::XMLError LoadScene(std::string name);
	std::string currentScene;

private:
	tinyxml2::XMLElement* saveNode(NodeComponent* parent, tinyxml2::XMLDocument* doc);
	std::vector<NodeComponent*> loadNodes(tinyxml2::XMLElement* start, tinyxml2::XMLElement* finish, NodeComponent* p);
	void deleteAllChildren(NodeComponent* children);

	std::string dir = "../Assets/Levels/";

};