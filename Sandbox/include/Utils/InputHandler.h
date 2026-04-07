//
// Created by denzel on 04/04/2025.
//

#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H
// #include "CameraController.h"
#include "DCraft.h"


class InputHandler {
public:
    // InputHandler(DCraft::Application* application, CameraController* camera_controller);
    void process(float delta_time);
private:
    // CameraController* camera_controller_;
    DCraft::Application* application_;
};



#endif //INPUTHANDLER_H
