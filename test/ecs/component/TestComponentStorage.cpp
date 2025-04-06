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
    storage.Clear<Position>();

    Entity e = 1;

    storage.Add<Position>(e, {1.0f, 2.0f, 3.0f});
    auto pos = storage.Get<Position>(e);

    REQUIRE(pos.has_value());
    REQUIRE(pos->x == Catch::Approx(1.0f));
    REQUIRE(pos->y == Catch::Approx(2.0f));
    REQUIRE(pos->z == Catch::Approx(3.0f));
}

TEST_CASE("ComponentStorage returns nullopt for missing component", "[ECS][ComponentStorage]")
{
    ComponentStorage storage;
    storage.Clear<Velocity>();

    Entity e = 42;

    auto result = storage.Get<Velocity>(e);
    REQUIRE_FALSE(result.has_value());
}

TEST_CASE("ComponentStorage stores components of different types separately", "[ECS][ComponentStorage]")
{
    ComponentStorage storage;
    storage.Clear<Position>();
    storage.Clear<Velocity>();

    Entity e1 = 1;
    Entity e2 = 2;

    storage.Add<Position>(e1, {1.0f, 1.0f, 1.0f});
    storage.Add<Velocity>(e2, {0.1f, 0.2f, 0.3f});

    auto pos = storage.Get<Position>(e1);
    auto vel = storage.Get<Velocity>(e2);

    REQUIRE(pos.has_value());
    REQUIRE(vel.has_value());

    REQUIRE(pos->x == Catch::Approx(1.0f));
    REQUIRE(vel->dz == Catch::Approx(0.3f));
}

TEST_CASE("ComponentStorage returns full map via GetAll", "[ECS][ComponentStorage]")
{
    ComponentStorage storage;
    storage.Clear<Velocity>();

    Entity e1 = 10;
    Entity e2 = 11;

    storage.Add<Velocity>(e1, {1, 2, 3});
    storage.Add<Velocity>(e2, {4, 5, 6});

    auto& all = storage.GetAll<Velocity>();
    REQUIRE(all.size() == 2);
    REQUIRE(all[e1].dx == Catch::Approx(1));
    REQUIRE(all[e2].dy == Catch::Approx(5));
}
