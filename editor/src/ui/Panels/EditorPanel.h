//
// Created by denzel on 01/10/2025.
//

#pragma once
#include <mutex>
#include <queue>

#include "hellfire/ecs/Entity.h"
#include "hellfire/platform/IWindow.h"
#include "hellfire/scene/Scene.h"
#include "hellfire/utilities/ServiceLocator.h"
#include "project/ProjectManager.h"
#include "ui/EventBus.h"
#include "utilities/MoveOnlyFunction.h"

namespace hellfire::editor {
    class EditorContext {
    public:
        EntityID  selected_entity_id = INVALID_ENTITY;
        Scene* active_scene = nullptr;
        EventBus event_bus;
        std::unique_ptr<ProjectManager> project_manager;

        // Thread-safe main thread queue
        void queue_main_thread(utils::MoveOnlyFunction fn) {
            std::lock_guard lock(main_thread_mutex_);
            main_thread_queue_.push(std::move(fn));
        }

        void process_main_thread_queue() {
            std::queue<utils::MoveOnlyFunction> to_process;

            {
                std::lock_guard lock(main_thread_mutex_);
                std::swap(to_process, main_thread_queue_);
            }
            while (!to_process.empty()) {
                to_process.front()();
                to_process.pop();
            }
        }

        void set_window_title(const std::string& title) {
            if (auto window = ServiceLocator::get_service<IWindow>()) {
                window->set_title(title + " - Hellfire Editor");
            }
        }

    private:
        std::mutex main_thread_mutex_;
        std::queue<utils::MoveOnlyFunction> main_thread_queue_;
    };
    
    class EditorPanel {
    protected:
        EditorContext* context_ = nullptr;
    public:
        virtual ~EditorPanel() = default;
        EditorPanel(EditorContext* ctx = nullptr) : context_(ctx) {}

        virtual void set_context(EditorContext* ctx) { context_ = ctx; }
        virtual void render() = 0;
        virtual void on_entity_selected(Entity* entity) {}
        
    };
}
