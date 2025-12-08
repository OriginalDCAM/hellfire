//
// Created by denzel on 05/12/2025.
//

#pragma once
#include "core/ApplicationState.h"

namespace hellfire::editor {
    
class ProjectLoadingState : public ApplicationState {
public:
    void on_enter() override;
    void render() override;

private:
    std::vector<std::string> log_messages_;
    float progress_ = 0.0f;
};
    
}
