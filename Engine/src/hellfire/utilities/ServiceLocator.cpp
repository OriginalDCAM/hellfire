//
// Created by denzel on 18/09/2025.
//
#include "hellfire/utilities/ServiceLocator.h"

namespace hellfire {
    std::unordered_map<std::type_index, void*> ServiceLocator::services_;

}
