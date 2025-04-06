#include <catch2/catch_all.hpp>

#include "ecs/ECS.hpp"
#include "ecs/system/ISystem.hpp"

using namespace mc::ecs;

struct MockComponent
{
    int value;
};

class MockSystem : public ISystem
{
public:
    void update(float deltaTime) override
    {
        updated = true;
        lastDelta = deltaTime;
    }

    bool updated = false;
    float lastDelta = 0.0f;
};

TEST_CASE("ECS creates entities with unique IDs", "[ECS]")
{
    ECS ecs;

    Entity e1 = ecs.createEntity();
    Entity e2 = ecs.createEntity();

    REQUIRE(e1 != INVALID_ENTITY);
    REQUIRE(e2 != INVALID_ENTITY);
    REQUIRE(e1 != e2);
}

TEST_CASE("ECS adds and retrieves components correctly", "[ECS]")
{
    ECS ecs;
    Entity e = ecs.createEntity();

    ecs.addComponent<MockComponent>(e, {42});

    auto compOpt = ecs.getComponent<MockComponent>(e);
    REQUIRE(compOpt.has_value());
    REQUIRE(compOpt->value == 42);
}

TEST_CASE("ECS getAllComponents returns correct map", "[ECS]")
{
    ECS ecs;
    Entity e1 = ecs.createEntity();
    Entity e2 = ecs.createEntity();

    ecs.addComponent<MockComponent>(e1, {1});
    ecs.addComponent<MockComponent>(e2, {2});

    auto& comps = ecs.getAllComponents<MockComponent>();
    REQUIRE(comps.size() == 2);
    REQUIRE(comps[e1].value == 1);
    REQUIRE(comps[e2].value == 2);
}

TEST_CASE("ECS systems update when ECS::update is called", "[ECS]")
{
    ECS ecs;
    auto system = std::make_shared<MockSystem>();

    ecs.addSystem(system);
    REQUIRE_FALSE(system->updated);

    ecs.update(0.016f);

    REQUIRE(system->updated);
    REQUIRE(system->lastDelta == Catch::Approx(0.016f));
}
