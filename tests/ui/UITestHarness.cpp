//
// Created by denzel on 10/12/2025.
//

#include "UITestHarness.h"

#include <imgui_te_ui.h>
#include <imgui_te_context.h>

#include "tests/ProjectCreatorTests.h"

namespace hellfire::tests {
    UITestHarness* UITestHarness::instance = nullptr;
    
    void UITestHarness::on_initialize(Application &app) {
        instance = this;
        
        EditorApplication::on_initialize(app);

        test_engine_ = ImGuiTestEngine_CreateContext();
        ImGuiTestEngineIO &io = ImGuiTestEngine_GetIO(test_engine_);

        io.ConfigVerboseLevel = ImGuiTestVerboseLevel_Info;
        io.ConfigVerboseLevelOnError = ImGuiTestVerboseLevel_Debug;
        io.ConfigRunSpeed = ImGuiTestRunSpeed_Fast;
        io.ConfigNoThrottle = true; // Run as fast as possible

        ImGuiTestEngine_Start(test_engine_, ImGui::GetCurrentContext());

        // Register all test suites
        RegisterProjectHubTests(test_engine_);
        RegisterProjectCreatorTests(test_engine_); 
        RegisterEditorStateTests(test_engine_);

        // Queue all tests
        ImGuiTestEngine_QueueTests(test_engine_, ImGuiTestGroup_Tests);
    }

    void UITestHarness::on_render() {
        EditorApplication::on_render();

#ifndef HELLFIRE_CI_BUILD
        ImGuiTestEngine_ShowTestEngineWindows(test_engine_, nullptr);
#endif

        // Check if tests are done
        if (!tests_complete_ && test_engine_) {
            ImGuiTestEngineIO &io = ImGuiTestEngine_GetIO(test_engine_);
            if (!io.IsRunningTests) {
                // Tests were queued and have finished running
                tests_complete_ = true;
            }
        }
    }

    void UITestHarness::on_end_frame() {
        if (test_engine_) {
            ImGuiTestEngine_PostSwap(test_engine_);
        }
        EditorApplication::on_end_frame();
    }

    int UITestHarness::get_exit_code() const {
        if (!test_engine_) return 1;

        int tested = 0, success = 0;
        ImGuiTestEngine_GetResult(test_engine_, tested, success);

        return (tested > 0 && tested == success) ? 0 : 1;
    }

    UITestHarness::~UITestHarness() {
        instance = nullptr;
        
        if (test_engine_) {
            ImGuiTestEngine_Stop(test_engine_);
            cleanup_imgui();
            ImGuiTestEngine_DestroyContext(test_engine_);
        }
    }



    void RegisterEditorStateTests(ImGuiTestEngine *engine) {
    }
}
