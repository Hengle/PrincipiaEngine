#ifndef NODECOMPONENT_H_
#define NODECOMPONENT_H_


#include <Artemis/Artemis.h>
#include <vector>
#include <queue>
namespace Principia {

	enum ComponentFlag {
		COMPONENT_NODE = 0x01,
		COMPONENT_TRANSFORM = 0x02,
		COMPONENT_MATERIAL = 0x04,
		COMPONENT_LIGHT = 0x08,
		COMPONENT_CAMERA = 0x10,
		COMPONENT_MODEL = 0x20,
		COMPONENT_MESH = 0x40,
		COMPONENT_BOX = 0x80,
		COMPONENT_SPHERE = 0x0100,
		COMPONENT_PLANE = 0x0200,
		COMPONENT_AABB = 0x0400,
		COMPONENT_CYLINDER = 0x0800,
		COMPONENT_SKINNED = 0x1000,
		COMPONENT_RIGIDBODY = 0x2000,
		COMPONENT_CCONTROLLER = 0x4000,
		COMPONENT_PRIMITIVE = 0x8000,
		COMPONENT_COLIDER = 0x10000,
		COMPONENT_IMPULSE = 0X20000,
		COMPONENT_GUI = 0X40000,
		COMPONENT_BUTTON = 0x80000,
		COMPONENT_JOINT = 0x100000
	};

	//enum Tagflags {
	//	TAG_NONE = 0x01,
	//	TAG_PLAYER = 0x02,
	//	TAG_WALL = 0x04,
	//	TAG_GOAL = 0x08,
	//	COMPONENT_SCENETRIGGER = 0x10
	//};

	enum class ObjectType {
		SPHERE = 1,
		BOX = 2,
		CYLINDER = 3,
		PLANE = 4
	};

	//typedef unsigned long ComponentFlag;
	struct NodeComponent : artemis::Component {
		artemis::Entity* data;
		NodeComponent*	 parent;
		std::vector<NodeComponent*> children;
		std::string name;
		bool clicked = false;
		bool isDynamic = false;
		bool isParent = false;
		int64_t engineFlags = COMPONENT_NODE;
		int64_t gameFlags = 0;

		NodeComponent(artemis::Entity* d) : data(d) { parent = this; };
		NodeComponent(artemis::Entity* d, NodeComponent* p) : data(d), parent(p) {};
		NodeComponent(artemis::Entity* d, NodeComponent* p, NodeComponent* child) : data(d), parent(p) { children.push_back(child); };
		NodeComponent(artemis::Entity* d, NodeComponent* p, NodeComponent copy) : parent(p), data(d), name(copy.name) { engineFlags = copy.engineFlags; };
		NodeComponent(artemis::Entity* d, std::string n, int64_t f) : data(d), name(n) { engineFlags |= f; parent = nullptr; };
	};
	
	//Returns a Breadth-First Vector of the nodes;
	inline void flatten(std::vector<NodeComponent*>& vec, NodeComponent* nc) {
		std::queue<NodeComponent*> q;
		q.push(nc);
		while (!q.empty()) {
			auto f = q.front();
			q.pop();
			for (auto child : f->children) {
				q.push(child);
				vec.push_back(child);
			}
		}
	}

	//Breadth First Graph Component
	//An array of node pointers layed out in BFS order
	struct BFGraphComponent : artemis::Component {
		std::vector<NodeComponent*> nodes;
	};

	// I wish i thought of this a loooong time ago
	// Adam Component: A Head Component in a node heirarchy
	// I chose the naem adam instead of parent since 
	// parents can technically be children but not adam
	struct AdamComponent : artemis::Component {
		size_t numChildren = 0;
		AdamComponent(size_t n) { numChildren = n; };
	};

}

#endif // !NODECOMPONENT_H_