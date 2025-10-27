//
// Created by denzel on 19/09/2025.
//

#include "hellfire/utilities/TextureUtils.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb/stb_image_resize2.h"

unsigned char* resize_image(unsigned char* input, int input_w, int input_h,
                           int output_w, int output_h, int channels, bool is_srgb = true) {
    unsigned char* output = new unsigned char[output_w * output_h * channels];
    
    stbir_pixel_layout layout;
    switch(channels) {
        case 1: layout = STBIR_1CHANNEL; break;
        case 2: layout = STBIR_2CHANNEL; break; 
        case 3: layout = STBIR_RGB; break;
        case 4: layout = STBIR_RGBA; break;
        default:
            delete[] output;
            return nullptr;
    }
    
    unsigned char* result;
    if (is_srgb) {
        result = stbir_resize_uint8_srgb(input, input_w, input_h, 0,
                                        output, output_w, output_h, 0, layout);
    } else {
        // Use linear for normal maps and data textures
        result = stbir_resize_uint8_linear(input, input_w, input_h, 0,
                                          output, output_w, output_h, 0, layout);
    }
    
    if (!result) {
        delete[] output;
        return nullptr;
    }
    
    return output;
}
