// Image module implementation

#include <arpa/inet.h> // htons
#include <string>
#include <cmath>

#include "Utility.hpp" // importing the utility module in include/ directory
#include "Image.hpp" // importing the image module in include/ directory

namespace kpeg
{
    Image::Image() : // constructor invoked
        width{0},
        height{0},
        m_pixelPtr{nullptr}
    {
        logFile << "Created new Image object" << std::endl; // for the log to output while execution
    }
    
    void Image::createImageFromMCUs(const std::vector<MCU>& MCUs)
    {
        logFile << "Creating Image from MCU vector..." << std::endl; // for the log to output while execution
        
        int mcuNum = 0; // the value of the MCU, Minimum Coded Units, which are compressed image tiles that are usually only 8x8 pixels in size
        
        int jpegWidth = width % 8 == 0 ? width : width + 8 - (width % 8);
        // calculating the width of the jpeg image
        int jpegHeight = height % 8 == 0 ? height : height + 8 - (height % 8);
        // calculating the height of the jpeg image
        
        // Create a pixel pointer of size (Image width) * (Image height)
        m_pixelPtr = std::make_shared<std::vector<std::vector<Pixel>>>(
            jpegHeight, std::vector<Pixel>(jpegWidth, Pixel()));
        
        // Populate the pixel pointer based on data from the specified MCUs
        for (int y = 0; y <= jpegHeight - 8; y += 8) // scanning the rows of the jpeg image
        {
            for (int x = 0; x <= jpegWidth - 8; x += 8) // scanning the cols of the jpeg image
            {
                auto pixelBlock = MCUs[mcuNum].getAllMatrices(); // function to get matrices
                
                for (int v = 0; v < 8; ++v) // scanning the MCU row wise
                {
                    for (int u = 0; u < 8; ++u) // scanning the MCU column wise
                    {
                        (*m_pixelPtr)[y + v][x + u].comp[0] = pixelBlock[0][v][u]; // R component of the pixel
                        (*m_pixelPtr)[y + v][x + u].comp[1] = pixelBlock[1][v][u]; // G component of the pixel
                        (*m_pixelPtr)[y + v][x + u].comp[2] = pixelBlock[2][v][u]; // B component of the pixel
                        /* In the above step, we are equating and decoding the pixel contents in the jepg image(Y, Cb, Cr) into R, G, B */
                    }
                }
            
                mcuNum++; // going to the next MCU or incrementing the number of MCUs
            }
        }
        
        // Trim the image width to nearest multiple of 8
        if (width != jpegWidth)
        {
            for (auto&& row : *m_pixelPtr)
                for (int c = 0; c < 8 - width % 8; ++c)
                    row.pop_back();
        }
        
        // Trim the image height to nearest multiple of 8
        if (height != jpegHeight)
        {
            for (int c = 0; c < 8 - height % 8; ++c)
                m_pixelPtr->pop_back();
        }        

        logFile << "Finished created Image from MCU [OK]" << std::endl; // completion message
    }
    
    const bool Image::dumpRawData(const std::string& filename)
    {
        if (m_pixelPtr == nullptr) // in case of error, the pixel pointer is missing
        {
            logFile << "Unable to create dump file \'" + filename + "\', Invalid pixel pointer" << std::endl;
            return false;
        }
        
        std::ofstream dumpFile(filename, std::ios::out);
        
        if (!dumpFile.is_open() || !dumpFile.good()) // if the dump file for the output is closed or corrupt, throw error
        {
            logFile << "Unable to create dump file \'" + filename + "\'." << std::endl;
            return false;
        }
        
        dumpFile << "P6" << std::endl;
        dumpFile << "# PPM dump created using libKPEG: https://github.com/TheIllusionistMirage/libKPEG" << std::endl;
        dumpFile << width << " " << height << std::endl;
        dumpFile << 255 << std::endl;
        
        for (auto&& row : *m_pixelPtr)
        {
            for (auto&& pixel : row)
                dumpFile << (UInt8)pixel.comp[RGBComponents::RED] // loading the R value in the onject to the dump file
                         << (UInt8)pixel.comp[RGBComponents::GREEN] // loading the G value in the onject to the dump file
                         << (UInt8)pixel.comp[RGBComponents::BLUE]; // loading the B value in the onject to the dump file
        }
        
        logFile << "Raw image data dumped to file: \'" + filename + "\'." << std::endl; // message of completion
        dumpFile.close(); // close the dump file
        return true; // return with no errors
    }
}
