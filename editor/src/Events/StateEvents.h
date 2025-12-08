//
// Created by denzel on 05/12/2025.
//

#pragma once

#include <filesystem>

#include "UI/EventBus.h"

namespace hellfire::editor {
    
    struct OpenProjectCreatorEvent {
        using is_event_tag = void;
    };
    
    struct CancelProjectCreatorEvent {
        using is_event_tag = void;
    };

    struct CreateProjectEvent {
        using is_event_tag = void;
        
        std::string name;
        std::filesystem::path location;
        std::string template_id;
    };

    struct OpenProjectEvent {
        using is_event_tag = void;
        
        std::filesystem::path path;
    };
    
    struct CloseProjectEvent {
        using is_event_tag = void;
    };

    struct  ProjectLoadProgressEvent {
        using is_event_tag = void;
        
        std::string message;
        float progress; // 0.0 to 1.0
    };

    struct ProjectLoadCompleteEvent {
        using is_event_tag = void;
        
    };
}
