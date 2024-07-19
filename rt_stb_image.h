#pragma once

#ifdef _MSC_VER
//  Forbidden all warning
    #pragma warning (push, 0)
#endif

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "stb_image.h"
#include "rt.h"

#include <cstdlib>
#include <iostream>
#include <string>


class rt_image
{
    private:
        const int      bytes_per_pixel    = 3;
        float*         fdata              = nullptr; // Linear floating point pixel - 32bit
        unsigned char* bdata              = nullptr; // Linear 8-bit pixel data
        int            image_width        = 0;       // Loaded image width
        int            image_height       = 0;       // Loaded image height
        int            bytes_per_scanline = 0;      
        std::string    filename; 

    static unsigned char float_to_byte(float value)
    {
        // Convert [0, 1] color to [0, 255]
        if(value <= 0.0) return 0;
        if(value >= 1.0) return 255;
        return static_cast<unsigned char>(256.0f * value);
    }

    void convert_to_bytes()
    {
        // Convert the linear floating point pixel data to bytes
        // Store the result bytes data in the bdata member

        int total_bytes = image_width * image_height * bytes_per_pixel;
        bdata = new unsigned char[total_bytes];

        // Iterate throught all pixel components, converting from [0.0, 1.0]
        // float values to unsigned [0, 255] byte values
        auto *bptr = bdata;
        auto *fptr = fdata;

        for(auto i = 0; i < total_bytes; i ++, fptr++, bptr++)
        {
            *bptr = float_to_byte(*fptr);
        }
    }
    
    public:
        rt_image() {}

        ~rt_image()
        {
            delete[] bdata;
            STBI_FREE(fdata);
        }

        rt_image(const rt_image& other)
        {
            filename           = other.filename;

            // Deep copy the pixel 
            load(filename.c_str());
        }

        rt_image& operator=(const rt_image& other)
        {
            if(this == &other) return *this;

            filename           = other.filename;

            load(filename.c_str());
        }

        rt_image(rt_image&& other) = delete;

        rt_image& operator=(rt_image&& other) = delete;

        explicit rt_image(const char* image_filename)
        {
            auto filename  = std::string(image_filename);
            this->filename = filename;
            auto imageDir  = getenv("RT_IMAGES");

            if(imageDir && load(std::string(imageDir) + "/" + filename)) return;
            if(load(filename)) return;
            if(load("images/" + filename)) return;
            if(load("../images/" + filename)) return;
            if(load("../../images/" + filename)) return;
            if(load("../../../images/" + filename)) return;
            if(load("../../../../images/" + filename)) return;

            std::cerr << "Failed to load image: " << image_filename << ".\n";
        }

        bool load(const std::string& filename)
        {
            auto n = bytes_per_pixel;
            fdata = stbi_loadf(filename.c_str(), &image_width, &image_height, &n, n);
            if(fdata == nullptr) return false;

            bytes_per_scanline = image_width * bytes_per_pixel;
            convert_to_bytes();
            return true;
        }

        int width() const {return (fdata == nullptr) ? 0 : image_width;}

        int height() const {return (fdata == nullptr) ? 0 : image_height;}

        const unsigned char* pixel_data(int x, int y) const 
        {
            static unsigned char default_pixel[3] = {255, 0, 255};
            if(bdata == nullptr) return default_pixel;

            x = clamp(x, 0, image_width);
            y = clamp(y, 0, image_height);

            return bdata + y * bytes_per_scanline + x * bytes_per_pixel;
        }
        
};


#ifdef _MSC_VER
// Recovery all exist warning
    #pragma warning (pop)
#endif