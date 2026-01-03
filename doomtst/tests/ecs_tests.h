#include "../game/ecs/game_object.h"
#include "test.h"
#pragma once
struct TestComponent :ecs::component {
	TestComponent() {

	}
};
static test::register_test ecs_basic_test("ECS Basic Functionality", []() -> test::test_result {
	// Create a small ECS
	ecs::Ecs ecs(10);

	// 1) Spawn an empty entity
	ecs::obj entity = ecs.spawn_empty();
	if (!ecs.contains(entity.inner())) return test::fail("Entity not valid after spawn");

	// 2) Add a simple component (we'll define a dummy for test)
	struct TestComponent {
		int x = 42;
	};
	entity.add_component<TestComponent>();

	if (!entity.has_component<TestComponent>())
		return test::fail("Entity does not have TestComponent after adding it");
	if (!ecs.archetypes.archetype_of(entity.inner()).elems) {

	}
	// 4) Remove component

	return test::pass();
	});