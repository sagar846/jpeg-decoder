#ifndef MCU_HPP
#define MCU_HPP

#include <array>
#include <vector>
#include <utility>

#include "Types.hpp"
#include "Transform.hpp"

namespace temp_peg
{

// Alias for a 8x8 pixel block with integral values for its channels
typedef std::array<std::array<std::array<int, 8>, 8>, 3> CompMatrices;

// Alias for a 8x8 matrix with integral elements
typedef std::array< std::array< int, 8 >, 8 > Matrix8x8;

class MCU
{
public:
    
    static int m_MCUCount;
    // m_QTables is the quantization tables used for quantization
    static std::vector<std::vector<UInt16>> m_QTables;

public:
    
    MCU();
    
    MCU(const std::array<std::vector<int>, 3>& compRLE,
        const std::vector<std::vector<UInt16>>& QTables);
    
    void constructMCU(const std::array<std::vector<int>, 3>& compRLE,
                        const std::vector<std::vector<UInt16>>& QTables);
    
    const CompMatrices& getAllMatrices() const;

private:
    
    void computeIDCT();
    
    void performLevelShift();
    
    void convertYCbCrToRGB();
    
private:
    
    CompMatrices m_block;
    int m_order;
    static int m_DCDiff[3];
    std::array<std::array<std::array<float, 8>, 8>, 3> m_IDCTCoeffs;
};

}
#endif //MCU HPP