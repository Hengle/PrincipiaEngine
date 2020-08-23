#pragma once
#include <Artemis/Artemis.h>
#include <Artemis/Entity.h>
#include <functional>

#include "transformComponent.hpp"
#include "nodeComponent.hpp"


namespace Principia {

	class Script {
	public: 
		Script() {};
		~Script() {};
		virtual void added() = 0;
		virtual void process(float dt) = 0;
		virtual void removed() = 0;
	protected:
		artemis::Entity* entity = nullptr;
	};

	class TestScript : public Script {
	public:
		void added() { std::cout << "Added"; }
		void process(float dt) {
			std::cout << "\nProcessing";
			auto tc = (TransformComponent*)entity->getComponent<TransformComponent>();
			std::cout << tc->world[3][0];
		};
		void removed() { std::cout << "removed"; };
		TestScript(artemis::Entity* e) { entity = e; }
	};

	struct ScriptComponent : artemis::Component {
		std::function<void(void)> added = nullptr;
		std::function<void(float)> process = nullptr;
		std::function<void(void)> removed = nullptr;
		Script* script;

		ScriptComponent(Script* s){
			script = s;
			added = [this]() {script->added(); };
			process = [this](float dt) {script->process(dt); };
			removed = [this]() {script->removed(); };
		};
		
	};

	struct HeartScript : public Script {
	public:
		void added() override {};
		void process(float dt) override {
			time += dt;
			auto* nc = (NodeComponent*)entity->getComponent<NodeComponent>();
			auto* tc = (TransformComponent*)nc->children[0]->data->getComponent<TransformComponent>();
			tc->eulerRotation.z = std::fma(30.f, dt, tc->eulerRotation.z);
			glm::mat4 rotM;
			rotM = glm::rotate(rotM, glm::radians(tc->eulerRotation.x), glm::vec3(1.f, 0.f, 0.f));
			rotM = glm::rotate(rotM, glm::radians(tc->eulerRotation.y), glm::vec3(0.f, 1.f, 0.f));
			rotM = glm::rotate(rotM, glm::radians(tc->eulerRotation.z), glm::vec3(0.f, 0.f, 1.f));

			tc->local.rotation = glm::toQuat(rotM);
			tc->local.position.y = std::fma(0.02f, glm::sin(time), tc->local.position.y);
		};
		void removed() override {};
		
		HeartScript(artemis::Entity* e) { entity = e; }
		HeartScript() {};
		~HeartScript() {}
	private: 
		float time = 0.f;
	};

}