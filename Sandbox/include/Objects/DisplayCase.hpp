//
// Created by denzel on 05/06/2025.
//
#include <DCraft.h>

#pragma once
class DisplayCase : public DCraft::Object3D {
public:
    void init();
    void update(float delta_time) override;
    
};

