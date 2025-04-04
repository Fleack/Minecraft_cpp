#pragma once

namespace mc::core
{
class Application
{
public:
    Application();
    ~Application();

    void Run();

private:
    bool Initialize();
    void Shutdown();
    void Update();
    void Render();
};
}
