/* 
 This is the types module
 This provides aliases and types:

 * UInt8, UInt16: unsigned integral types 8-bits and 16-bits wide
 * RGBComponents: identifying the components of RGB colour model pixels
 * Pixel: represents a three-channel pixel with discrete integral range of channel intensities
 * PixelPtr: represents a 2D array of Pixels
 * HuffmanTable: represents a Huffman table with upto 16 length symbols.
*/

#ifndef TYPES_HPP // code inside #ifdef and #endif will be taken for compilation only if TYPES_HPP is defined

#define TYPES_HPP

#include <vector> // contains dynamic arrays
#include <array> // contains fixed sized arrays
#include <utility>
#include <memory>

namespace kpeg {
    /* 
    Standard, cross-platform word sizes for channels
    The channels we deal with here are Red, Green and Blue channels
    in the RGB color model  and the Y, Cb and Cr channels in the
    Y-Cb-Cr color model.
    */

    // Standard unsigned integral types
    typedef unsigned char  UInt8;  // defining type UInt8 as unsigned char
    typedef unsigned short UInt16;  // defining type UInt16 as unsigned short
    
    // Standard signed integral types
    typedef char  Int8;  // defining type Int8 as char
    typedef short Int16;  // defining type Int8 as short
    
    // enumerate RGB Components into R, G, B for identifying channels in the colour model
    enum RGBComponents {
        RED   ,
        GREEN ,
        BLUE
    };
    
    // Pixel types
    // The pixel types used here use three channels    
    struct Pixel {
        // Default constructor
        // By default a pixel is initialized to its lowest brightness value, which is zero
        Pixel() {
            comp[0] = 0; // setting initial values to the components
            comp[1] = 0;
            comp[2] = 0;
        }
        
        // Parameterized constructor
        // comp0 - Intensity value of pixel component 0
        // comp1 - Intensity value of pixel component 1
        // comp2 - Intensity value of pixel component 2
        Pixel(const Int16 comp0, const Int16 comp1, const Int16 comp2) {
            comp[0] = comp0;
            comp[1] = comp1;
            comp[2] = comp2;
        }
        
        // Store the intensity of the pixel
        Int16 comp[3];
    };
    
    // Aliases for commonly used types

    // A 2D array of pixels with discrete components
    typedef std::shared_ptr<std::vector<std::vector<Pixel>>>  PixelPtr;
    // shared_ptr is a smart pointer that retains shared ownership of an object through a pointer
    // vector is a container that encapsulates dynamic size arrays
    // both of them have a scope of std, so std :: is required. Hence a namespace std is given in the module
    // Now all of these properties are make alias to PixelPtr, which represents a 2D array of pixels

    // Huffman table
    typedef std::array<std::pair<int, std::vector<UInt8>>, 16> HuffmanTable;
    // std :: array is a container that encapsulates fixed size arrays
    // std :: pair is a container defined in <utility> which consists of two data elements
    // The above make up the aliases fro the huffman table

    
    // Identifiers used to access a Huffman table based on the class and ID
    // E.g., To access the Huffman table for the DC coefficients of the
    // Cb Cr component, we use `huff_table[HT_DC][HT_CbCr]`.
    const int HT_DC   = 0;
    const int HT_AC   = 1;
    const int HT_Y    = 0;
    const int HT_CbCr = 1;
}

#endif
