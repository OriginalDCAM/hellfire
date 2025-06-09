//
// Created by denzel on 05/06/2025.
//
#include <DCraft.h>

#pragma once
class DisplayCase : public DCraft::Object3D {
public:
    DisplayCase() : Object3D("Display Case") {}
    void setup() override;
    void update(float delta_time) override;
    
};

