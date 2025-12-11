//
// Created by denzel on 05/12/2025.
//

#pragma once
#include "hellfire/ecs/Entity.h"

namespace hellfire::editor {
    class EditorContext;
    
    class ApplicationState {
    public:
        virtual ~ApplicationState() = default;

        virtual void on_enter() {}
        virtual void on_exit() {}
        virtual void render() = 0;

        // Input handling - return true if consumed
        virtual bool on_mouse_move(float x, float y, float x_offset, float y_offset) { return false; }
        virtual bool on_mouse_button(int button, bool pressed) { return false; }
        virtual bool on_mouse_wheel(float delta) { return false; }
        virtual bool on_key_down(int key) { return false; }
        virtual bool on_key_up(int key) { return false; }
    
        // Optional overrides
        virtual Entity* get_render_camera_override() { return nullptr; }

        void set_context(EditorContext* ctx) { context_ = ctx; }

    protected:
        EditorContext* context_ = nullptr;
    };
}
