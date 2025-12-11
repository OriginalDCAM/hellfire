//
// Created by denzel on 10/12/2025.
//

#pragma once

#include "core/EditorApplication.h"
#include "imgui_te_engine.h"

namespace hellfire::tests {
    class UITestHarness : public editor::EditorApplication {
    public:
        static UITestHarness* instance;
        
        void on_initialize(Application& app) override;
        void on_render() override;
        void on_end_frame() override;
    
        bool is_complete() const { return tests_complete_; }
        int get_exit_code() const;
    
        ImGuiTestEngine* get_test_engine() const { return test_engine_; }
        editor::EditorContext* get_editor_context() { return &editor_context_; }
    
        ~UITestHarness() override;

    private:
        ImGuiTestEngine* test_engine_ = nullptr;
        bool tests_complete_ = false;
    };

    // Test registration functions
    void RegisterEditorStateTests(ImGuiTestEngine* engine);

} // namespace hellfire::tests

