#include <catch2/catch_all.hpp>

#include "ecs/Entity.hpp"
#include "ecs/component/ComponentStorage.hpp"

using namespace mc::ecs;

struct Position
{
    float x, y, z;
};

struct Velocity
{
    float dx, dy, dz;
};

TEST_CASE("ComponentStorage adds and retrieves components", "[ECS][ComponentStorage]")
{
    ComponentStorage storage;
    storage.clear<Position>();

    Entity e = 1;

    storage.add<Position>(e, {1.0f, 2.0f, 3.0f});
    auto pos = storage.get<Position>(e);

    REQUIRE(pos.has_value());
    REQUIRE(pos->x == Catch::Approx(1.0f));
    REQUIRE(pos->y == Catch::Approx(2.0f));
    REQUIRE(pos->z == Catch::Approx(3.0f));
}

TEST_CASE("ComponentStorage returns nullopt for missing component", "[ECS][ComponentStorage]")
{
    ComponentStorage storage;
    storage.clear<Velocity>();

    Entity e = 42;

    auto result = storage.get<Velocity>(e);
    REQUIRE_FALSE(result.has_value());
}

TEST_CASE("ComponentStorage stores components of different types separately", "[ECS][ComponentStorage]")
{
    ComponentStorage storage;
    storage.clear<Position>();
    storage.clear<Velocity>();

    Entity e1 = 1;
    Entity e2 = 2;

    storage.add<Position>(e1, {1.0f, 1.0f, 1.0f});
    storage.add<Velocity>(e2, {0.1f, 0.2f, 0.3f});

    auto pos = storage.get<Position>(e1);
    auto vel = storage.get<Velocity>(e2);

    REQUIRE(pos.has_value());
    REQUIRE(vel.has_value());

    REQUIRE(pos->x == Catch::Approx(1.0f));
    REQUIRE(vel->dz == Catch::Approx(0.3f));
}

TEST_CASE("ComponentStorage returns full map via GetAll", "[ECS][ComponentStorage]")
{
    ComponentStorage storage;
    storage.clear<Velocity>();

    Entity e1 = 10;
    Entity e2 = 11;

    storage.add<Velocity>(e1, {1, 2, 3});
    storage.add<Velocity>(e2, {4, 5, 6});

    auto& all = storage.getAll<Velocity>();
    REQUIRE(all.size() == 2);
    REQUIRE(all[e1].dx == Catch::Approx(1));
    REQUIRE(all[e2].dy == Catch::Approx(5));
}
