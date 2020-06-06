#pragma once

#include "System.h"

class GravitySystem : public System<MotionComponent, GravityComponent> {
public:
	virtual void update() override {
		//std::cout << "Gravity[" << _entities.size() << "],";
		for (auto& entityID : _entities) {
			Entity& e = getEntity(entityID);
			MotionComponent* motion = e.getComponent<MotionComponent>();
			GravityComponent* gravity = e.getComponent<GravityComponent>();
			motion->_velocity += gravity->_direction * gravity->_g;
		}
	}
};