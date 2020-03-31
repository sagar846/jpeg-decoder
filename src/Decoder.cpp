void Decoder :: parseSOSSegment() {
        if (!m_imageFile.is_open() || !m_imageFile.good()) { // check if file is open and has proper integrity
            logFile << "Unable scan image file: \'" + m_filename + "\'" << std :: endl;
            return;
        }
        
        logFile << "Parsing SOS segment..." << std :: endl;
        
        UInt16 len;
        
        m_imageFile.read(reinterpret_cast<char *>(&len), 2);
        len = htons(len); // swaps the endianness of shorts (host-to-network short)
        
        logFile << "SOS segment length: " << len << std :: endl;
        
        UInt8 compCount; // Number of components
        UInt16 compInfo; // Component ID and Huffman table used
        
        m_imageFile >> std :: noskipws >> compCount; // load the value, do not skip white spaces
        
        if (compCount < 1 || compCount > 4) {
            logFile << "Invalid component count in image scan: " << (int)compCount << ", terminating decoding process..." << std :: endl;
            return;
        }
        
        logFile << "Number of components in scan data: " << (int)compCount << std :: endl;
        
        for (auto i = 0; i < compCount; ++i) {
            m_imageFile.read(reinterpret_cast<char *>(&compInfo), 2);
            compInfo = htons(compInfo); // swaps the endianness of shorts
            
            UInt8 cID = compInfo >> 8; // 1st byte denotes component ID 
            
            // 2nd byte denotes the Huffman table used:
            // Bits 7 to 4: DC Table #(0 to 3)
            // Bits 3 to 0: AC Table #(0 to 3)
            UInt8 DCTableNum = (compInfo & 0x00f0) >> 4;
            UInt8 ACTableNum = (compInfo & 0x000f);
            
            logFile << "Component ID: " << (int)cID << ", DC Table #: " << (int)DCTableNum << ", AC Table #: " << (int)ACTableNum << std :: endl;
        }
        
        // Skip the next three bytes
        for (auto i = 0; i < 3; ++i) {
            UInt8 byte;
            m_imageFile >> std :: noskipws >> byte;
        }
        
        logFile << "Finished parsing SOS segment [OK]" << std :: endl;
        
        scanImageData();
    }
    
void Decoder :: scanImageData() {
        if (!m_imageFile.is_open() || !m_imageFile.good()) {
            logFile << "Unable scan image file: \'" + m_filename + "\'" << std :: endl;
            return;
        }
        
        logFile << "Scanning image data..." << std :: endl;
        
        UInt8 byte;
        
        while (m_imageFile >> std :: noskipws >> byte) { // do not skip white spaces
            if (byte == JFIF_BYTE_FF) { // all the markers start with JFIF_BYTE_FF  as the MSB, defined in markers.hpp
                UInt8 prevByte = byte;
                
                m_imageFile >> std :: noskipws >> byte;
                
                if (byte == JFIF_EOI) {
                    logFile << "Found segment, End of Image (FFD9)" << std :: endl;
                    return;
                }
                
                std :: bitset<8> bits1(prevByte);
                logFile << "0x" << std :: hex << std :: setfill('0') << std :: setw(2)
                                            << std :: setprecision(8) << (int)prevByte
                                            << ", Bits: " << bits1 << std :: endl;
                                          
                m_scanData.append(bits1.to_string());
            }
            
            std :: bitset<8> bits(byte);
            logFile << "0x" << std :: hex << std :: setfill('0') << std :: setw(2)
                                        << std :: setprecision(8) << (int)byte
                                        << ", Bits: " << bits << std :: endl;
            
            m_scanData.append(bits.to_string());
        }
        
        logFile << "Finished scanning image data [OK]" << std :: endl;
}
