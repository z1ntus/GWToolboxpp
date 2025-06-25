#pragma once

#include <ToolboxUIPlugin.h>

#include <GWCA/Managers/AgentMgr.h>

//20250624

#include <queue>
#include <functional>

struct MoveCommand {
    float x, y;
    MoveCommand(float x, float y) : x(x), y(y) {}
};

class ExamplePlugin : public ToolboxPlugin {
public:
    ExamplePlugin() = default;
    ~ExamplePlugin() override = default;

    const char* Name() const override { return "Example Plugin"; }

    void LoadSettings(const wchar_t*) override;
    void SaveSettings(const wchar_t*) override;
    [[nodiscard]] bool HasSettings() const override { return true; }
    void DrawSettings() override;
    void Initialize(ImGuiContext* ctx, ImGuiAllocFns allocator_fns, HMODULE toolbox_dll) override;
    void SignalTerminate() override;
    bool CanTerminate() override;
    // Draw user interface. Will be called every frame if the element is visible
    void Draw(IDirect3DDevice9* pDevice) override;
    //20250624
    void Update(float delta) override;
//20250624
private:
    std::queue<MoveCommand> move_queue;
    bool is_moving = false;
    const float reach_threshold = 50.0f; // Adjust as needed


};
