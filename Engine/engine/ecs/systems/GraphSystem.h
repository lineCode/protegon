#pragma once

#include "System.h"

#include "utils/Vector2.h"
#include "renderer/Renderer.h"
#include "renderer/TextureManager.h"
#include <deque>

constexpr int AXIS_OFFSET = 50;

class GraphSystem : public ecs::System<TransformComponent, PlayerController, RigidBodyComponent> {
public:
	GraphSystem(engine::Renderer renderer, V2_int graph) : renderer{ renderer }, graph_size{ graph.x - AXIS_OFFSET, graph.y } {
		points.resize(graph_size.x);
	}
	virtual void Update() override final {
		renderer.Clear();
		for (auto [entity, transform, player, rb] : entities) {
			points.pop_front();
			//points.push_back(transform.rotation);
			points.push_back(rb.rigid_body.acceleration.y / 20);
		}
		for (int i = 0; i < graph_size.x; ++i) {
			assert(i < points.size() && "Cannot draw point outside range");
			auto x = AXIS_OFFSET + i;
			auto y = points[i] + graph_size.y / 2;
			auto point = V2_int{ static_cast<int>(x), static_cast<int>(y) };
			engine::TextureManager::DrawPoint(renderer, point, engine::RED);
		}
		engine::TextureManager::DrawLine(renderer, V2_int{ AXIS_OFFSET, graph_size.y / 2 }, V2_int{ AXIS_OFFSET + graph_size.x, graph_size.y / 2 });
		engine::TextureManager::DrawLine(renderer, V2_int{ AXIS_OFFSET, 0 }, V2_int{ AXIS_OFFSET, graph_size.y });
		engine::TextureManager::SetDrawColor(renderer, engine::TextureManager::GetDefaultRendererColor());
		renderer.Present();
	}
private:
	V2_int graph_size;
	std::deque<double> points;
	engine::Renderer renderer;
};