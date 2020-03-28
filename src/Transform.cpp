/* 
Transform module implementation

Used functions:
* matIndicesToZZOrder: converts a matrix index, (i,j), to its corresponding order, in zig-zag ordering
* zzOrderToMatIndices: converts a zig-zag order, to its corresponding matrix index, (i,j)
* bitStringToValue: convert a bit string representation to its corresponding value
* getValueCategory: get the category of a value
*/
#include <cmath>

#include "Transform.hpp" // The library defined in the include/ directory
using namespace std;

namespace temp_peg {

    // This converts a zig zag order to its matrix string
    const pair <const int, const int> zzOrderToMatIndices( const int zzindex ) { 
        static const pair <const int, const int> zzorder[64] = {
            {0,0},
            {0,1}, {1,0},         
            {2,0}, {1,1}, {0,2},
            {0,3}, {1,2}, {2,1}, {3,0},
            {4,0}, {3,1}, {2,2}, {1,3}, {0,4},
            {0,5}, {1,4}, {2,3}, {3,2}, {4,1}, {5,0},
            {6,0}, {5,1}, {4,2}, {3,3}, {2,4}, {1,5}, {0,6},
            {0,7}, {1,6}, {2,5}, {3,4}, {4,3}, {5,2}, {6,1}, {7,0},
            {7,1}, {6,2}, {5,3}, {4,4}, {3,5}, {2,6}, {1,7},
            {2,7}, {3,6}, {4,5}, {5,4}, {6,3}, {7,2},
            {7,3}, {6,4}, {5,5}, {4,6}, {3,7},
            {4,7}, {5,6}, {6,5}, {7,4},
            {7,5}, {6,6}, {5,7},
            {6,7}, {7,6},
            {7,7}
        };
        
        return zzorder[zzindex];
    }
    
    // This converts a matrix string to its zig zag order
    const int matIndicesToZZOrder( const int row, const int column ) { 
        static const int matOrder[8][8] =  {
            {  0,  1,  5,  6, 14, 15, 27, 28 },
            {  2,  4,  7, 13, 16, 26, 29, 42 },
            {  3,  8, 12, 17, 25, 30, 41, 43 },
            {  9, 11, 18, 24, 31, 40, 44, 53 },
            { 10, 19, 23, 32, 39, 45, 52, 54 },
            { 20, 22, 33, 38, 46, 51, 55, 60 },
            { 21, 34, 37, 47, 50, 56, 59, 61 },
            { 35, 36, 48, 49, 57, 58, 62, 63 }
        };
        
        return matOrder[row][column];
    }

    // bitStr defined in Types module, return value of type Int16
    const Int16 bitStringtoValue(const string& bitStr) { 
        if (bitStr == "")
            return 0x0000;
        
        Int16 value = 0x0000;
        
        char sign = bitStr[0];
        int factor = sign == '0' ? -1 : 1;
            
        for (auto i = 0; i < bitStr.size(); ++i) {
            if (bitStr[i] == sign)
                value += Int16(pow(2, bitStr.size() - 1 - i));
        }
        
        return factor * value;
    }
    
    const Int16 getValueCategory(const Int16 value) {
        if (value == 0x0000)
            return 0;
        return log2(abs(value)) + 1;
    }
}
