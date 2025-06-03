#pragma once

#include "ecs/Ecs.hpp"
#include "ecs/system/ISystem.hpp"

#include <Magnum/Math/Vector2.h>

namespace mc::ecs {

class CameraInputSystem final : public ISystem {
public:
    explicit CameraInputSystem(Ecs& ecs);

    void update(float) override;
    void render(float) override {}

    void handleMouse(Magnum::Vector2 const& delta);

private:
    Ecs& m_ecs;
};

} // namespace mc::ecs
