//
// Created by denzel on 06/12/2025.
//

#pragma once
#include <memory>
#include <utility>

namespace hellfire::editor::utils {


    class MoveOnlyFunction {
    public:
        MoveOnlyFunction() = default;
    
        template<typename F>
        MoveOnlyFunction(F&& f) 
            : impl_(std::make_unique<Model<F>>(std::forward<F>(f))) {}
    
        MoveOnlyFunction(MoveOnlyFunction&&) = default;
        MoveOnlyFunction& operator=(MoveOnlyFunction&&) = default;
    
        // Non-copyable
        MoveOnlyFunction(const MoveOnlyFunction&) = delete;
        MoveOnlyFunction& operator=(const MoveOnlyFunction&) = delete;
    
        void operator()() {
            if (impl_) impl_->call();
        }
    
        explicit operator bool() const { return impl_ != nullptr; }

    private:
        struct Concept {
            virtual ~Concept() = default;
            virtual void call() = 0;
        };
    
        template<typename F>
        struct Model : Concept {
            F func;
            Model(F&& f) : func(std::forward<F>(f)) {}
            void call() override { func(); }
        };
    
        std::unique_ptr<Concept> impl_;
    }; 
}