//
// Created by denzel on 22/10/2025.
//

#pragma once

namespace hellfire {
    struct Time {
        inline static float delta_time = 0.0f;
        inline static float last_frame_time = 0.0f;
        inline static float current_time = 0.0f;

        // Store the start time
        inline static std::chrono::time_point<std::chrono::high_resolution_clock> start_time;

        static void init() {
            start_time = std::chrono::high_resolution_clock::now();
        }

        static void update() {
            auto now = std::chrono::high_resolution_clock::now();
            std::chrono::duration<float> elapsed = now - start_time;
            current_time = elapsed.count();

            delta_time = current_time - last_frame_time;
            last_frame_time = current_time;
        }
    };
}