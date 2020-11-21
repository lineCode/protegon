#pragma once

#include <engine/Include.h>

ecs::Entity CreatePlayer(V2_double position, V2_double size, ecs::Manager& manager) {
	auto entity = manager.CreateEntity();
	V2_double player_acceleration = { 2, 2 };
	entity.AddComponent<TransformComponent>(position);
	entity.AddComponent<InputComponent>();
	entity.AddComponent<PlayerController>(player_acceleration);
	entity.AddComponent<RigidBodyComponent>(RigidBody{ UNIVERSAL_DRAG, GRAVITY, 5, abs(player_acceleration) + abs(GRAVITY) });
	entity.AddComponent<CollisionComponent>(position, size);
	//entity.AddComponent<SpriteComponent>("./resources/textures/hopper.png", V2_int{ 20 * 2, 23 * 2 });
	entity.AddComponent<SpriteSheetComponent>();
	//entity.AddComponent<StateMachineComponent>(entity, RawStateMachineMap{ { "walkStateMachine", new WalkStateMachine("idle") }, { "jumpStateMachine", new JumpStateMachine("grounded") }});
	entity.AddComponent<DirectionComponent>();
	entity.AddComponent<AnimationComponent>();
	entity.AddComponent<RenderComponent>();
	//entity.AddComponent<CameraComponent>(engine::Camera{}, true);
	return entity;
}