//
// Created by denzel on 09/04/2025.
//

#include "EditorCommand.h"
#include "DCraft/Addons/SceneManager.h"

namespace DCraft::Editor {
class RemoveObjectCommand : public EditorCommand {
public:
    RemoveObjectCommand(SceneManager& sm, Object3D* parent, Object3D* obj);
    void execute() override;
    void undo() override;
private:
    SceneManager& scene_manager_;
    Object3D* parent_;
    Object3D* object_;
};
    
}
