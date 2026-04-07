//
// Created by denzel on 03/04/2025.
//

#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H
#include <string>
#include <unordered_map>

#include "DCraft/Graphics/Textures/Texture.h"

namespace DCraft {
    class TextureManager {
    public:
        static TextureManager &get_instance();

        Texture *get_texture(const std::string &path);

        void add_texture(const std::string &path, Texture *texture) { textures_[path] = texture; }

        void clear();

        ~TextureManager() {
            clear();
        }

    private:
        static TextureManager *instance_;

        TextureManager() {
        }

        std::unordered_map<std::string, Texture*> textures_;
    };

}


#endif //TEXTUREMANAGER_H
