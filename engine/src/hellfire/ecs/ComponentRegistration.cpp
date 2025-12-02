//
// Created by denzel on 30/11/2025.
//

#include "ComponentRegistration.h"

#include "ComponentRegistry.h"
#include "Entity.h"
#include "TransformComponent.h"

namespace hellfire {
    void register_all_components() {
        auto &reg = ComponentRegistry::instance();

        reg.register_component<TransformComponent>("TransformComponent",
                                                   [](const TransformComponent &t) {
                                                       return nlohmann::json{
                                                           {
                                                               "position",
                                                               {
                                                                   t.get_position().x, t.get_position().y,
                                                                   t.get_position().z
                                                               }
                                                           },
                                                           {
                                                               "scale",
                                                               {t.get_scale().x, t.get_scale().y, t.get_scale().z}
                                                           },
                                                           {
                                                               "rotation",
                                                               {
                                                                   t.get_rotation().x, t.get_rotation().y,
                                                                   t.get_rotation().z
                                                               }
                                                           }
                                                       };
                                                   },
                                                   [](Entity *e, const nlohmann::json &j) {
                                                       const auto t = e->transform();

                                                       if (j.contains("position")) {
                                                           auto &pos = j.at("position");
                                                           t->set_position({pos[0], pos[1], pos[2]});
                                                       }

                                                       if (j.contains("scale")) {
                                                           auto &scale = j.at("scale");
                                                           t->set_scale({scale[0], scale[1], scale[2]});
                                                       }

                                                       if (j.contains("rotation")) {
                                                           auto &rot = j.at("rotation");
                                                           t->set_rotation({rot[0], rot[1], rot[2]});
                                                       }
                                                   });
    }
}
