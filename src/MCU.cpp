/*
The properties imported from mcu.hpp are:

* m_block: 8 × 8 block of pixels that make up the MCU
* m_order: the order of the MCU in the image
* m_IDCTCoeffs: the MCU after performing IDCT
* m_DCDiff: the differences in DC consecutive coefficients per channel which is shared by all MCUs
* m_QTables: the quantization tables used for quantization which is shared by all MCUs and publicly available
* m_MCUCount: the total number of MCUs in the image which is shared by all MCUs and publicly available

The functions imported from mcu.hpp are:

* constructMCU: create the MCU from the specified run-length encoding and quantization tables
* getAllMatrices: get the pixel arrays for the pixels under this MCU
* computeIDCT: compute IDCT
* performLevelShift: level shift the pixel data to center it within the pixel value range
* convertYCbCrToRGB: convert the MCU’s underlying pixels from the Y-Cb-Cr colour model to RGB colour model
*/

#include <string>
#include <sstream>
#include <cmath>
#include <iomanip>
#include <iostream>

#include "Utility.hpp"
#include "MCU.hpp"
using namespace std;

namespace temp_peg {
    int MCU :: m_MCUCount = 0;
    vector <vector <UInt16> > MCU :: m_QTables = {}; // initialize a default constructor
    int MCU :: m_DCDiff[3] = { 0, 0, 0 }; // initialise all the coeffs in different channels to 0
    MCU :: MCU() {} // initialize a default constructor
            
    MCU :: MCU(const array <vector <int>, 3> &compRLE, const vector <vector <UInt16> > &QTables) {
        constructMCU(compRLE, QTables);
    }
    
    void MCU :: constructMCU(const array <vector <int>, 3> &compRLE, const vector <vector <UInt16> > &QTables) {
        m_QTables = QTables;
        
        m_MCUCount++;
        m_order = m_MCUCount;
        
        logFile << "Constructing MCU: " << dec << m_order << "..." << endl;
        
        const char* component[] = { "Y (Luminance)", "Cb (Chrominance)", "Cr (Chrominance)" }; // making an object for jpeg elements
        const char* type[] = { "DC", "AC" }; // it consists of Ac and DC coeffs
        
        for ( int compID = 0; compID < 3; compID++ )
        {
            // Initialize with all zeros
            array<int, 64> zzOrder;            
            fill( zzOrder.begin(), zzOrder.end(), 0 ); // from the beginning to the end, fill it with 0
            int j = -1;
            
            for ( auto i = 0; i <= compRLE[compID].size() - 2; i += 2 ) { // auto keyword automatically initialises the variable types required;
                if ( compRLE[compID][i] == 0 && compRLE[compID][i + 1] == 0 )
                    break;
                
                j += compRLE[compID][i] + 1; // Skip the number of positions containing zeros, j contains it
                zzOrder[j] = compRLE[compID][i + 1];
            }
            
            // formula to get the DC coeff
            // DC_i = DC_i-1 + DC-difference
            m_DCDiff[compID] += zzOrder[0];
            zzOrder[0] = m_DCDiff[compID];
            
            int QIndex = compID == 0 ? 0 : 1;
            for ( auto i = 0; i < 64; ++i ) 
                zzOrder[i] *= m_QTables[QIndex][i];
            
            // Zig-zag order to 2D matrix order
            for ( auto i = 0; i < 64; ++i ) {
                auto coords = zzOrderToMatIndices(i);
                
                m_block[compID][coords.first][coords.second] = zzOrder[i];
            }
        }
        
        computeIDCT();
        performLevelShift();
        convertYCbCrToRGB();
        
        logFile << "Finished constructing MCU: " << m_order << "..." << endl;
    }
    
    const CompMatrices& MCU::getAllMatrices() const {
        return m_block;
    }
    
    void MCU::computeIDCT() {
        logFile << "Performing IDCT on MCU: " << m_order << "..." << endl;
        
        for ( int i = 0; i < 3; ++i ) { // 3 channels in .
            for ( int y = 0; y < 8; ++y ) { // 8 x 8 channels in a MCU
                for ( int x = 0; x < 8; ++x ) {
                    float sum = 0.0;
                    
                    for ( int u = 0; u < 8; ++u ) {
                        for ( int v = 0; v < 8; ++v ) {
                            float Cu = u == 0 ? 1.0 / sqrt(2.0) : 1.0;
                            float Cv = v == 0 ? 1.0 / sqrt(2.0) : 1.0;

                            // m_block, i^th channel, (y * x)^th MCU, calculating DCT
                            sum += Cu * Cv * m_block[i][u][v] * cos( ( 2 * x + 1 ) * u * M_PI / 16.0 ) *
                                            cos( ( 2 * y + 1 ) * v * M_PI / 16.0 );
                        }
                    }
                    
                    m_IDCTCoeffs[i][x][y] = 0.25 * sum; // Finding inverse DCT
                }
            }
        }

        logFile << "IDCT of MCU: " << m_order << " complete [OK]" << endl; // loggint the idct
    }
    
    void MCU::performLevelShift() {
        logFile << "Performing level shift on MCU: " << m_order << "..." << endl;
        
        for ( int i = 0; i <3; ++i ) {
            for ( int y = 0; y < 8; ++y ) {
                for ( int x = 0; x < 8; ++x ) { // level shifting, as in adding 128 to each element in the array
                    m_block[i][y][x] = roundl( m_IDCTCoeffs[i][y][x] ) + 128;
                }
            }
        }
        
        logFile << "Level shift on MCU: " << m_order << " complete [OK]" << endl;
    }
    
    void MCU :: convertYCbCrToRGB() {
        logFile << "Converting from Y-Cb-Cr colorspace to R-G-B colorspace for MCU: " << m_order << "..." << endl; // decoding the Y, Cb, Cr to RGB
        
        for ( int y = 0; y < 8; ++y ) { // 8 x 8 channels in a MCU
            for ( int x = 0; x < 8; ++x ) {
                float Y = m_block[0][y][x]; // equate Y to first channel
                float Cb = m_block[1][y][x]; // equate Cb to second channel
                float Cr = m_block[2][y][x]; // equate Cr to third channel
                
                // using the formula told, find the corresponding RGB value from the Y, Cb, Cr values, and typecast it to int for safety
                int R = (int)floor( Y + 1.402 * ( 1.0 * Cr - 128.0 ) );
                int G = (int)floor( Y - 0.344136 * ( 1.0 * Cb - 128.0 ) - 0.714136 * ( 1.0 * Cr - 128.0 ) );
                int B = (int)floor( Y + 1.772 * ( 1.0 * Cb - 128.0 ) );
                
                // filter the value of each pixel
                R = max( 0, min( R, 255 ) );
                G = max( 0, min( G, 255 ) );
                B = max( 0, min( B, 255 ) );
                
                // finally equate the RGB values to the pixel array
                m_block[0][y][x] = R;
                m_block[1][y][x] = G;
                m_block[2][y][x] = B;
            }
        }
        
        logFile << "Colorspace conversion for MCU: " << m_order << " done [OK]" << endl; // log that conversion has been done
    }
}
