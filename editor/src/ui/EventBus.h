//
// Created by denzel on 05/12/2025.
//

#pragma once
#include <functional>
#include <memory>
#include <typeindex>
#include <unordered_map>


namespace hellfire::editor {
    template<typename T>
    concept IsEvent = requires
    {
        typename T::is_event_tag;
    };

    class EventBus {
    public:
        using EventSubscribers = std::unordered_map<std::type_index, std::vector<std::function<void(const void *)> > >;
        using EventQueue = std::vector<std::pair<std::type_index, std::shared_ptr<void> > >;

        template<IsEvent T>
        using Callback = std::function<void(const T &)>;

        template<IsEvent T>
        void subscribe(Callback<T> callback) {
            auto &vec = subscribers_[typeid(T)];
            vec.push_back([cb = std::move(callback)](const void *event) {
                cb(*static_cast<const T *>(event));
            });
        }

        template<IsEvent T>
        void publish(const T &event) {
            const auto it = subscribers_.find(typeid(T));
            if (it == subscribers_.end()) return;

            for (auto &fn: it->second) {
                fn(&event);
            }
        }

        template<IsEvent T, typename... Args>
        void dispatch(Args &&... args) {
            publish(T{std::forward<Args>(args)...});
        }

        template<IsEvent T>
        void queue(const T &event) {
            queue_.emplace_back(typeid(T), std::make_shared<T>(event));
        }

        void dispatch_queued() {
            for (auto &[type, event_ptr]: queue_) {
                if (auto it = subscribers_.find(type); it != subscribers_.end()) {
                    for (auto &fn: it->second) {
                        fn(event_ptr.get());
                    }
                }
            }
            queue_.clear();
        }

    private:
        EventSubscribers subscribers_;
        EventQueue queue_;
    };
}
