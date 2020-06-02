#pragma once
#include "Component.h"
#include "./Vec2D.h"

struct TransformComponent : public Component {
	static ComponentID ID;
	Vec2D _position;
	float _scale;
	float _rotation;
	TransformComponent(Vec2D position = Vec2D(), float scale = 1.0f, float rotation = 0.0f) : _position(position), _rotation(rotation), _scale(scale) {
		ID = createComponentID<TransformComponent>();
	}
};

ComponentID TransformComponent::ID = 0;