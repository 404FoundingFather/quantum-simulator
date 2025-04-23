#include "UIManager.h"
#include <imgui.h>

UIManager::UIManager() {}
UIManager::~UIManager() {}

void UIManager::render() {
    ImGui::Begin("Simulation Controls");
    // TODO: add controls (Start, Stop, Reset, sliders, dropdowns)
    ImGui::End();
}

void UIManager::updateConfig(const PhysicsConfig& config) {
    // TODO: handle updated configuration parameters
}