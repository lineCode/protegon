#include <engine/Include.h>

#include "components/Components.h"
#include "event/Events.h"
#include "factory/Factories.h"
#include "systems/Systems.h"

class MyGame : public engine::Engine {
public:
	std::vector<engine::Image> images;
	//V2_int tiles_per_chunk = { 16, 16 };
	//V2_int tile_size = { 32, 32 };
	V2_int tiles_per_chunk = { 8, 8 };
	V2_int tile_size = { 64, 64 };
	void Init() {

		LOG("Initializing game systems...");
		scene.manager.AddSystem<RenderSystem>(&scene);
		scene.manager.AddSystem<HitboxRenderSystem>(&scene);
		scene.manager.AddSystem<PhysicsSystem>();
		scene.manager.AddSystem<TargetSystem>();
		scene.manager.AddSystem<LifetimeSystem>();
		scene.manager.AddSystem<AnimationSystem>();
		scene.manager.AddSystem<CollisionSystem>();
		scene.manager.AddSystem<InputSystem>();
		scene.manager.AddSystem<StateMachineSystem>();
		scene.manager.AddSystem<DirectionSystem>();
		scene.manager.AddSystem<CameraSystem>(&scene);
		scene.ui_manager.AddSystem<RenderSystem>();
		scene.ui_manager.AddSystem<UIButtonListener>(&scene);
		scene.ui_manager.AddSystem<UIButtonRenderer>();
		scene.ui_manager.AddSystem<UITextRenderer>();

		//LOG("Sectors: " << Engine::ScreenSize() / tile_size);

		title_screen = scene.event_manager.CreateEntity();
		engine::EventHandler::Register<TitleScreenEvent>(title_screen);
		auto& title = title_screen.AddComponent<TitleScreenComponent>();
		pause_screen = scene.event_manager.CreateEntity();
		engine::EventHandler::Register<PauseScreenEvent>(pause_screen);
		auto& pause = pause_screen.AddComponent<PauseScreenComponent>();
		engine::EventHandler::Invoke(title_screen, scene.manager, scene.ui_manager);
		title.open = true;
		pause.open = false;
		LOG("Initialized all game systems successfully");
		
	}
	struct cmp {
		bool operator() (AABB a, AABB b) const {
			return a == b;
		}
	};
	// TODO: Move chunks vector elsewhere.
	std::vector<engine::Chunk*> chunks;
	bool Contains(std::vector<engine::Chunk*> vector, AABB value) {
		for (auto& v : vector) {
			if (v->GetInfo() == value) { return true; }
		}
		return false;
	}
	bool Contains(std::vector<AABB> vector, AABB value) {
		for (auto& v : vector) {
			if (v == value) { return true; }
		}
		return false;
	}
	int octave = 5;
	double bias = 2.0;
	engine::Chunk* player_chunk = nullptr;
    void Update() {
		auto players = scene.manager.GetComponentTuple<TransformComponent, CollisionComponent, PlayerController>();
		auto& pause = pause_screen.GetComponent<PauseScreenComponent>();
		scene.manager.Update<InputSystem>();
		if (!pause.open) {
			//engine::Timer timer9;
			//timer9.Start();
			scene.ui_manager.Update<UIButtonListener>();
			if (scene.manager.HasSystem<PhysicsSystem>()) {
				scene.manager.Update<PhysicsSystem>();
				scene.manager.Update<TargetSystem>();
			}
			scene.manager.Update<CollisionSystem>();
			if (player_chunk != nullptr) {
				auto [entity, transform, collision, player] = players[0];
				auto chunk_entities = player_chunk->manager.GetComponentTuple<TransformComponent, CollisionComponent>();
				chunk_entities.emplace_back(entity, transform, collision);
				//CollisionRoutine(chunk_entities);
			}
			scene.manager.Update<StateMachineSystem>();
			scene.manager.Update<DirectionSystem>();
			scene.manager.Update<LifetimeSystem>();
			scene.manager.Update<CameraSystem>();
			//LOG("All systems: " << timer9.ElapsedMilliseconds());
		}
		//AllocationMetrics::PrintMemoryUsage();
		auto& title = title_screen.GetComponent<TitleScreenComponent>();
		if (engine::InputHandler::KeyPressed(Key::R)) {
			engine::EventHandler::Invoke(title_screen, scene.manager, scene.ui_manager);
			title.open = true;
			pause.open = false;
		} else if (title.open) {
			if (scene.ui_manager.GetEntitiesWith<TitleScreenComponent>().size() == 0) {
				title.open = false;
			}
		}

		if (engine::InputHandler::KeyDown(Key::X))
			octave++;

		if (engine::InputHandler::KeyDown(Key::C))
			octave--;

		if (engine::InputHandler::KeyDown(Key::F))
			bias += 0.2;

		if (engine::InputHandler::KeyDown(Key::G))
			bias -= 0.2;

		if (bias < 0.2)
			bias = 0.2;

		if (octave < 1)
			octave = 1;

		//static int cn = 0;
		//if (cn % 10 == 0)
		//	;
		//	//LOG("octave: " << octave << ", bias: " << bias);
		//cn++;
		// TODO: Massive cleanup...
		engine::Timer timer0;
		timer0.Start();
		auto camera = scene.GetCamera();
		if (camera && !title.open) {
			V2_double chunk_size = tiles_per_chunk * tile_size;
			V2_double lowest = Floor(camera->offset / chunk_size);
			V2_double highest = Floor((camera->offset + static_cast<V2_double>(engine::Engine::ScreenSize()) / camera->scale) / chunk_size);
			assert(lowest.x <= highest.x && "Left grid edge cannot be above right grid edge");
			assert(lowest.y <= highest.y && "Top grid edge cannot be below top grid edge");
			// Optional: Expand loaded chunk region.
			//lowest += -1;
			//highest += 1;
			//std::vector<AABB> potential_chunks;
			auto camera_box = AABB{ lowest * chunk_size, Abs(highest - lowest + V2_double{ 1, 1 }) * chunk_size };

			// Remove old chunks that have gone out of range of camera.
			auto it = chunks.begin();
			while (it != chunks.end()) {
				auto chunk = (*it);
				auto chunk_box = chunk->GetInfo();
				chunk_box.size *= tile_size;
				if (!chunk || !engine::collision::AABBvsAABB(chunk_box, camera_box)) {
					delete chunk;
					it = chunks.erase(it);
				} else {
					++it;
				}
			}

			// Go through all new chunks.
			for (auto i = lowest.x; i != highest.x + 1; ++i) {
				for (auto j = lowest.y; j != highest.y + 1; ++j) {
					// AABB corresponding to the potentially new chunk.
					auto potential_chunk = AABB{ chunk_size * V2_double{ i, j }, tiles_per_chunk };
					bool new_chunk = true;
					for (auto c : chunks) {
						if (c) {
							// Check if chunk exists already, if so, skip it.
							if (c->GetInfo() == potential_chunk) {
								c->new_chunk = false;
								new_chunk = false;
								break;
							}
						}
					}
					if (new_chunk) {
						auto chunk = new BoxChunk();
						chunk->Init(potential_chunk, tile_size, &scene);
						engine::Timer timer;
						timer.Start();
						chunk->Generate(1, octave, bias);
						chunk->new_chunk = true;
						chunks.push_back(chunk);
					}
				}
			}
			for (auto c : chunks) {
				if (c) {
				auto chunk_box = c->GetInfo();
				chunk_box.size *= tile_size;
				// Check if chunk is player chunk.
					for (auto [entity, transform, collision, player] : players) {
						if (engine::collision::AABBvsAABB(collision.collider, chunk_box)) {
							player_chunk = c;
						}
					}
				}
			}
			//LOG("Chunks: " << chunks.size());
		} else {
			for (auto c : chunks) {
				if (c) {
					delete c;
					c = nullptr;
				}
			}
			chunks.clear();
		}
		auto time = timer0.ElapsedMilliseconds();
		if (time > 1) {
			//LOG("Chunks took: " << time);
		}
		//LOG("Octave: " << octave << ", bias: " << bias);
		if (engine::InputHandler::KeyPressed(Key::ESCAPE) && pause.toggleable && !title.open) {
			pause.toggleable = false;
			engine::EventHandler::Invoke(pause_screen, pause_screen, scene.manager, scene.ui_manager);
		} else if (engine::InputHandler::KeyReleased(Key::ESCAPE)) {
			if (!pause.toggleable) {
				pause.release_time += 1;
			}
			if (pause.release_time >= 5) {
				pause.toggleable = true;
				pause.release_time = 0;
			}
		}
    }
	void Render() {
		//engine::Timer timer0;
		//timer0.Start();
		auto& pause = pause_screen.GetComponent<PauseScreenComponent>();
		if (!pause.open) {
			scene.manager.Update<AnimationSystem>();




		}
		//LOG("Animations: " << timer0.ElapsedMilliseconds());
		// TODO: Consider a more automatic way of doing this?
		engine::Timer timer;
		timer.Start();
		auto camera = scene.GetCamera();
		if (camera) {
			for (auto& c : chunks) {
				if (c->new_chunk) {
					c->Update();
					c->new_chunk = false;
				}
				auto chunk_box = c->GetInfo();
				chunk_box.size *= tile_size;
				c->Render();
				engine::TextureManager::DrawRectangle((chunk_box.position - camera->offset) * camera->scale, Ceil(chunk_box.size * camera->scale), engine::PURPLE);
				/*c->manager.Update<HitboxRenderSystem>();
				c->manager.Update<RenderSystem>();*/
			}
		}
		//LOG("Rendered " << 256 * chunks.size() << " hitboxes");
		auto time = timer.ElapsedMilliseconds();
		if (true) {
			LOG("Rendering " << 256 * chunks.size() << " tiles took " << time << " milliseconds");
		}
		scene.manager.Update<RenderSystem>();
		scene.manager.Update<HitboxRenderSystem>();

		scene.ui_manager.Update<UIButtonRenderer>();
		scene.ui_manager.Update<UITextRenderer>();





	}
private:
	ecs::Entity title_screen;
	ecs::Entity pause_screen;
};

int main(int argc, char* args[]) { // sdl main override

	LOG("Starting Protegon");
	engine::Engine::Start<MyGame>("Protegon", 512 * 2, 600, 60);

    return 0;
}