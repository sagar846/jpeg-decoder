#ifndef DECODER_HPP
#define DECODER_HPP

#include <fstream>
#include <vector>
#include <utility>
#include <bitset>

#include "Types.hpp"
#include "Image.hpp"
#include "HuffmanTree.hpp"
#include "MCU.hpp"

namespace temp_peg
{

class Decoder
{
    public:

        enum ResultCode
        {
            SUCCESS,
            TERMINATE,
            ERROR,
            DECODE_INCOMPLETE,
            DECODE_DONE
        };
        
    public:
        
        Decoder();
        
        Decoder(const std::string& filename);
        
        ~Decoder();
        
        bool open(const std::string& filename);
        
        ResultCode decodeImageFile();

        bool dumpRawData();

        void close();
                    
    private:

        ResultCode parseSegmentInfo(const UInt8 byte);
        
        void parseAPP0Segment();

        void parseCOMSegment();
        
        void parseDQTSegment();
        
        ResultCode parseSOF0Segment();
        
        void parseDHTSegment();
        
        void parseSOSSegment();
        
        void scanImageData();
        
        void byteStuffScanData();
        
        void decodeScanData();
        
    private:
        
        std::string m_filename;
        std::ifstream m_imageFile;
        Image m_image;
        std::vector<std::vector<UInt16>> m_QTables;
        HuffmanTable m_huffmanTable[2][2];
        std::vector< std::pair<int, int> > mDHTsScanned;
        HuffmanTree m_huffmanTree[2][2];
        std::string m_scanData;
        std::vector<MCU> m_MCU;
};
}

#endif // DECODER_HPP
