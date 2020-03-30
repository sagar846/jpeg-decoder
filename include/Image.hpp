#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <memory>

#include "Types.hpp"
#include "MCU.hpp"

namespace temp_peg
{    

class Image
{
    public:
        
        Image();
        
        void createImageFromMCUs(const std::vector<MCU>& MCUs);
        
        const bool dumpRawData(const std::string& filename);
        
    public:
        
        std::size_t width;
        std::size_t height;

    private:
        PixelPtr m_pixelPtr;
};
}

#endif // IMAGE_HPP