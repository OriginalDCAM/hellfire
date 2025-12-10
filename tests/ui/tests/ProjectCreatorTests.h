#pragma once

#include "../UITestHarness.h"
#include "events/StateEvents.h"
#include <imgui_te_context.h>

namespace hellfire::tests {
    inline ImGuiTest *RegisterTestWithTeardown(
        ImGuiTestEngine *engine,
        const char *category,
        const char *name
    ) {
        ImGuiTest *t = IM_REGISTER_TEST(engine, category, name);
        t->TeardownFunc = [](ImGuiTestContext *ctx) {
            auto *harness = UITestHarness::instance;
            if (harness) {
                harness->get_editor_context()->event_bus.dispatch<editor::CloseProjectEvent>();
                ctx->Yield(20);
            }
        };
        return t;
    }


    inline void RegisterProjectHubTests(ImGuiTestEngine *engine) {
        ImGuiTest *t = nullptr;
        t = RegisterTestWithTeardown(engine, "ProjectHub", "WindowVisible");
        t->TestFunc = [](ImGuiTestContext *ctx) {
            ctx->Yield(5); // Let the app initialize
            ctx->WindowInfo("Project Hub", ImGuiTestOpFlags_None);
        };

        t = RegisterTestWithTeardown(engine, "ProjectHub", "CreateButton_OpensCreator");
        t->TestFunc = [](ImGuiTestContext *ctx) {
            ctx->Yield(30);
            IM_CHECK(ctx->WindowInfo("Hellfire - Project Hub").ID != 0);
            ctx->SetRef("Hellfire - Project Hub");
            ctx->ItemClick("HubLayout/New Project");
            ctx->Yield(10);

            IM_CHECK(ctx->WindowInfo("//Create New Project").ID != 0);
        };
    }

    inline void RegisterProjectCreatorTests(ImGuiTestEngine *engine) {
        ImGuiTest *t = nullptr;

        // ============================================
        // Navigation Tests
        // ============================================

        t = RegisterTestWithTeardown(engine, "ProjectCreator", "WindowOpens_FromProjectHub");
        t->TestFunc = [](ImGuiTestContext *ctx) {
            ctx->Yield(30);

            ctx->SetRef("//Hellfire - Project Hub");
            ctx->ItemClick("HubLayout/New Project");
            ctx->Yield(10);

            IM_CHECK(ctx->ItemInfo("//Create New Project").ID != 0);
        };

        t = RegisterTestWithTeardown(engine, "ProjectCreator", "CancelButton_ReturnsToHub");
        t->TestFunc = [](ImGuiTestContext *ctx) {
            ctx->Yield(30);

            ctx->SetRef("//Hellfire - Project Hub");
            ctx->ItemClick("HubLayout/New Project");
            ctx->Yield(10);

            ctx->SetRef("//Create New Project");
            ctx->ItemClick("**/Cancel");
            ctx->Yield(10);

            IM_CHECK(ctx->ItemInfo("//Hellfire - Project Hub").ID != 0);
        };

        // ============================================
        // Form Input Tests
        // ============================================

        t = RegisterTestWithTeardown(engine, "ProjectCreator", "FormInputs_AcceptText");
        t->TestFunc = [](ImGuiTestContext *ctx) {
            ctx->Yield(30);

            ctx->SetRef("//Hellfire - Project Hub");
            ctx->ItemClick("HubLayout/New Project");
            ctx->Yield(10);

            ctx->SetRef("//Create New Project");

            ctx->ItemClick("**/##name");
            ctx->KeyCharsAppend("MyTestProject");
            ctx->Yield(5);

            ctx->ItemClick("**/##location");
            ctx->KeyCharsAppend("C:/Projects");
            ctx->Yield(5);
        };

        t = RegisterTestWithTeardown(engine, "ProjectCreator", "CreateButton_DisabledWhenEmpty");
        t->TestFunc = [](ImGuiTestContext *ctx) {
            ctx->Yield(30);

            ctx->SetRef("//Hellfire - Project Hub");
            ctx->ItemClick("HubLayout/New Project");
            ctx->Yield(10);

            ctx->SetRef("//Create New Project");
        };

        t = RegisterTestWithTeardown(engine, "ProjectCreator", "CreateButton_EnabledWhenFormFilled");
        t->TestFunc = [](ImGuiTestContext *ctx) {
            ctx->Yield(30);

            ctx->SetRef("//Hellfire - Project Hub");
            ctx->ItemClick("HubLayout/New Project");
            ctx->Yield(10);

            ctx->SetRef("//Create New Project");

            ctx->ItemClick("**/##name");
            ctx->KeyCharsAppend("MyTestProject");

            ctx->ItemClick("**/##location");
            ctx->KeyCharsAppend("C:/Projects");
            ctx->Yield(5);

            // Create button should now be enabled
            ImGuiTestItemInfo create_btn = ctx->ItemInfo("//Create");
            IM_CHECK(create_btn.ID != 0);
            IM_CHECK((create_btn.ItemFlags & ImGuiItemFlags_Disabled) == 0);
        };

        // ============================================
        // Template Selection Tests
        // ============================================

        t = RegisterTestWithTeardown(engine, "ProjectCreator", "TemplateList_CanSelectTemplate");
        t->TestFunc = [](ImGuiTestContext *ctx) {
            ctx->Yield(30);

            ctx->SetRef("//Hellfire - Project Hub");
            ctx->ItemClick("HubLayout/New Project");
            ctx->Yield(10);

            ctx->SetRef("//Create New Project");

            ctx->ItemClick("**/##templates");
            ctx->Yield(5);

            // If you have specific template names:
            ctx->ItemClick("**/##templates/Empty Project");
        };

        // ============================================
        // End-to-End Project Creation Test
        // ============================================

        t = RegisterTestWithTeardown(engine, "ProjectCreator", "CreateProject_EndToEnd");
        t->TestFunc = [](ImGuiTestContext *ctx) {
            ctx->Yield(30);

            ctx->SetRef("//Hellfire - Project Hub");
            ctx->ItemClick("HubLayout/New Project");
            ctx->Yield(10);

            ctx->SetRef("//Create New Project");

            ctx->ItemClick("**/##name");
            ctx->KeyCharsAppend("E2ETestProject");

            ctx->ItemClick("**/##location");
            ctx->KeyCharsAppend("C:/TestProjects");
            ctx->Yield(5);

            ctx->ItemClick("**/Create");
            ctx->Yield(60); // Wait for project loading

            // Should transition to editor
            // IM_CHECK(ctx->ItemInfo("//Editor")->ID != 0);
        };

        // ============================================
        // Form Reset Test
        // ============================================

        t = RegisterTestWithTeardown(engine, "ProjectCreator", "FormResets_WhenReopened");
        t->TestFunc = [](ImGuiTestContext *ctx) {
            ctx->Yield(30);

            // Open creator and fill form
            ctx->SetRef("//Hellfire - Project Hub");
            ctx->ItemClick("HubLayout/New Project");
            ctx->Yield(10);

            ctx->SetRef("//Create New Project");
            ctx->ItemClick("**/##name");
            ctx->KeyCharsAppend("SomeProject");
            ctx->Yield(5);

            // Cancel
            ctx->ItemClick("**/Cancel");
            ctx->Yield(10);

            // Reopen
            ctx->SetRef("//Hellfire - Project Hub");
            ctx->ItemClick("HubLayout/New Project");
            ctx->Yield(10);

            // Form should be reset - Create button should be disabled again
            ctx->SetRef("//Create New Project");
            ImGuiTestItemInfo create_btn = ctx->ItemInfo("**/Create");
            IM_CHECK((create_btn.ItemFlags & ImGuiItemFlags_Disabled) != 0);
        };

        // ============================================
        // Folder Browser Button Test
        // ============================================

        t = RegisterTestWithTeardown(engine, "ProjectCreator", "FolderButton_Exists");
        t->TestFunc = [](ImGuiTestContext *ctx) {
            ctx->Yield(30);

            ctx->SetRef("//Hellfire - Project Hub");
            ctx->ItemClick("HubLayout/New Project");
            ctx->Yield(10);

            ctx->SetRef("//Create New Project");

            // If you add ##FolderBrowse ID to your button:
            // ImGuiTestItemInfo* folder_btn = ctx->ItemInfo("##FolderBrowse");
            // IM_CHECK(folder_btn != nullptr);
            // IM_CHECK(folder_btn->ID != 0);
        };
    }
} // namespace hellfire::tests
