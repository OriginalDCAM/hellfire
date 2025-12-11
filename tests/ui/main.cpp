//
// Created by denzel on 10/12/2025.
//
#include "UITestHarness.h"
#include "hellfire/core/Application.h"


int main(int argc, char** argv) {
    bool exit_on_completion = false;
    
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--exit-on-completion") {
            exit_on_completion = true;
        }
    }
    
    hellfire::Application app;
    auto test_harness = std::make_unique<hellfire::tests::UITestHarness>();
    auto* harness = test_harness.get();  // Get raw pointer before move
    app.register_plugin(std::move(test_harness));
    
    if (exit_on_completion) {
        app.set_exit_condition([harness]() {
            return harness->is_complete();
        });
    }

    app.initialize();
    app.run();
    
    int result = harness->get_exit_code();
    
    std::cout << "\n========================================\n";
    std::cout << "UI Tests " << (result == 0 ? "PASSED" : "FAILED") << "\n";
    std::cout << "========================================\n";
    
    return result;
}