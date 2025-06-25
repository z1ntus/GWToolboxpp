#include "ExamplePlugin.h"

#include <GWCA/Constants/Constants.h>
#include <GWCA/Managers/MapMgr.h>
#include <GWCA/Utilities/Hook.h>

#include <Utils/GuiUtils.h>
#include "GWCA/Managers/ChatMgr.h"

//20250624
//#include <GWCA/Managers/Agents.h> // For GW::Agents::GetPlayer()
#include <imgui.h>                // For ImGui::Text etc.
#include <GWCA/GameEntities/Agent.h>



namespace {
    bool redirect_slash_ee_to_eee = false;
    GW::HookEntry ChatCmd_HookEntry;
}

DLLAPI ToolboxPlugin* ToolboxPluginInstance()
{
    static ExamplePlugin instance;
    return &instance;
}

void ExamplePlugin::LoadSettings(const wchar_t* folder)
{
    ToolboxPlugin::LoadSettings(folder);
    PLUGIN_LOAD_BOOL(redirect_slash_ee_to_eee);
}

void ExamplePlugin::SaveSettings(const wchar_t* folder)
{
    PLUGIN_SAVE_BOOL(redirect_slash_ee_to_eee);
    ToolboxPlugin::SaveSettings(folder);
}

void ExamplePlugin::DrawSettings()
{
    if (!toolbox_handle) {
        return;
    }
    ImGui::Checkbox("Redirect ee to eee", &redirect_slash_ee_to_eee);
    
    if (ImGui::Button("Start Move Sequence")) {
        while (!move_queue.empty())
            move_queue.pop(); // Clear existing
        move_queue.push({15515.f, 6000.f});
        move_queue.push({16064.f, 5270.f});
        is_moving = false; // Reset
    }
    //20250624
    //if (ImGui::Button("Move to (15515, 6000)")) {
    //    GW::Agents::Move(15515.f, 6000.f);
    //}
    //20250624
    const GW::Agent* agent = GW::Agents::GetObservingAgent();
    if (agent) {
        ImGui::Text("Your position:");
        ImGui::Text("X: %.2f", agent->pos.x);
        ImGui::Text("Y: %.2f", agent->pos.y);
    }
    else {
        ImGui::Text("Agent not available.");
    }
}

void EeCmd(GW::HookStatus*, const wchar_t*, const int, const LPWSTR*)
{
    if (redirect_slash_ee_to_eee) {
        GW::Chat::SendChat('/', "eee");
    }
}

void ExamplePlugin::Initialize(ImGuiContext* ctx, const ImGuiAllocFns allocator_fns, const HMODULE toolbox_dll)
{
    ToolboxPlugin::Initialize(ctx, allocator_fns, toolbox_dll);
    GW::Chat::CreateCommand(&ChatCmd_HookEntry, L"ee", EeCmd);
}

void ExamplePlugin::SignalTerminate()
{
    ToolboxPlugin::SignalTerminate();
    GW::Chat::DeleteCommand(&ChatCmd_HookEntry);
}

bool ExamplePlugin::CanTerminate() {
    return true;
}

void ExamplePlugin::Draw(IDirect3DDevice9*)
{
    if (GW::Map::GetInstanceType() != GW::Constants::InstanceType::Loading) {
        return;
    }

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
    if (ImGui::Begin(Name(), GetVisiblePtr(), ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar)) {
        ImGui::Text("Example plugin: area loading...");
    }
    ImGui::End();
    ImGui::PopStyleColor();
}

//20250624
void ExamplePlugin::Update(float delta)
{
    (void)delta; // explicitly mark as unused
    if (move_queue.empty()) return;

    GW::Agent* player = GW::Agents::GetObservingAgent();
    if (!player) return;

    if (!is_moving) {
        // Start moving to next target
        auto& target = move_queue.front();
        GW::Agents::Move(target.x, target.y);
        is_moving = true;
    }
    else {
        // Check if we've reached the destination
        auto& target = move_queue.front();
        float dx = target.x - player->pos.x;
        float dy = target.y - player->pos.y;
        float dist_sq = dx * dx + dy * dy;

        if (dist_sq < reach_threshold * reach_threshold) {
            move_queue.pop();
            is_moving = false;
        }
    }
}
