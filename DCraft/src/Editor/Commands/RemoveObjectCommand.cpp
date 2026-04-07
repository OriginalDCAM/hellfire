//
// Created by denzel on 09/04/2025.
//
#include "DCraft/Editor/Commands/RemoveObjectCommand.h"
#include "DCraft/Addons/SceneManager.h"
#include "DCraft/Structs/Scene.h"

namespace DCraft::Editor {
    RemoveObjectCommand::RemoveObjectCommand(SceneManager& sm, Object3D *parent,
        Object3D *obj) : scene_manager_(sm), parent_(parent), object_(obj) {}

    void RemoveObjectCommand::execute() {
        if (parent_) {
            parent_->remove(object_);
        } else {
            scene_manager_.get_active_scene()->remove(object_);
        }
    }

    void RemoveObjectCommand::undo() {
        if (parent_) {
            parent_->add(object_);
        } else {
            scene_manager_.get_active_scene()->add(object_);
        }
    }
}
