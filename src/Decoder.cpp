// Parse the start of file segment

Decoder::ResultCode Decoder::parseSOF0Segment()
    {
                                                                       //Check whether image file is open or state of file is good.
        if (!m_imageFile.is_open() || !m_imageFile.good())      
        {
            logFile << "Unable scan image file: \'" + m_filename + "\'" << std::endl;
            return ResultCode::ERROR;
        }
        
        logFile << "Parsing SOF-0 segment..." << std::endl;
        
        UInt16 lenByte, imgHeight, imgWidth;
        UInt8 precision, compCount;
        
        m_imageFile.read(reinterpret_cast<char *>(&lenByte), 2);
        lenByte = htons(lenByte);                                //htons function converts values from host to network byte order
        
        logFile << "SOF-0 segment length: " << (int)lenByte << std::endl;       
        
        m_imageFile >> std::noskipws >> precision;                                      // It does not skip whitespace before next input and
        logFile << "SOF-0 segment data precision: " << (int)precision << std::endl;     // precision controlls the total number of digits that are printed.
        
        m_imageFile.read(reinterpret_cast<char *>(&imgHeight), 2);                      // image height and width
        m_imageFile.read(reinterpret_cast<char *>(&imgWidth), 2);
        
        imgHeight = htons(imgHeight);
        imgWidth = htons(imgWidth);
        
        logFile << "Image height: " << (int)imgHeight << std::endl;
        logFile << "Image width: " << (int)imgWidth << std::endl;
        
        m_imageFile >> std::noskipws >> compCount;                              //Extracting no. of components from image file
        
        logFile << "No. of components: " << (int)compCount << std::endl;            
        
        UInt8 compID = 0, sampFactor = 0, QTNo = 0;
        
        bool isNonSampled = true;
        
        for (auto i = 0; i < 3; ++i)
        {
            m_imageFile >> std::noskipws >> compID >> sampFactor >> QTNo;       // Extracting component ID ,sampling Factor and Quantization table of image
            
            logFile << "Component ID: " << (int)compID << std::endl;
            logFile << "Sampling Factor, Horizontal: " << int(sampFactor >> 4) << ", Vertical: " << int(sampFactor & 0x0F) << std::endl;
            logFile << "Quantization table no.: " << (int)QTNo << std::endl;
            
            if ((sampFactor >> 4) != 1 || (sampFactor & 0x0F) != 1)
                isNonSampled = false;
        }
        
        if (!isNonSampled)
        {
            logFile << "Chroma subsampling not yet supported!" << std::endl;
            logFile << "Chroma subsampling is not 4:4:4, terminating..." << std::endl;
            return ResultCode::TERMINATE;
        }
        
        logFile << "Finished parsing SOF-0 segment [OK]" << std::endl;        
        m_image.width = imgWidth;                                                   // Image height and width  in pixels
        m_image.height = imgHeight;
        
        return ResultCode::SUCCESS;
    }
    
//Parse the Huffman tables defined in the DHT segment.

    void Decoder::parseDHTSegment()
    {   
                                                                     //Check whether image file is open or state of file is good.
        
        if (!m_imageFile.is_open() || !m_imageFile.good())                          
        {
            logFile << "Unable scan image file: \'" + m_filename + "\'" << std::endl;
            return;
        }
        
        logFile << "Parsing Huffman table segment..." << std::endl;
        
        UInt16 len;
        m_imageFile.read(reinterpret_cast<char *>(&len), 2);
        len = htons(len);
        
        logFile << "Huffman table length: " << (int)len << std::endl;           //Prints huffman table length
        
        int segmentEnd = (int)m_imageFile.tellg() + len - 2;                    //Gives current position of get pointer
        
        while (m_imageFile.tellg() < segmentEnd)
        {
            UInt8 htinfo;
            m_imageFile >> std::noskipws >> htinfo;
            
            int HTType = int((htinfo & 0x10) >> 4);
            int HTNumber = int(htinfo & 0x0F);
            
            logFile << "Huffman table type: " << HTType << std::endl;
            logFile << "Huffman table #: " << HTNumber << std::endl;
            
            int totalSymbolCount = 0;
            UInt8 symbolCount;
                                                                                //Count total number of symbols
            for (auto i = 1; i <= 16; ++i)
            {
                m_imageFile >> std::noskipws >> symbolCount;
                m_huffmanTable[HTType][HTNumber][i-1].first = (int)symbolCount;
                totalSymbolCount += (int)symbolCount;
            }
            
                                                                                // Load the symbols
            int syms = 0;
            for (auto i = 0; syms < totalSymbolCount; )
            {
                /* Read the next symbol, and add it to the proper slot in the Huffman table.
                 Depndending upon the symbol count, say n, for the current
                 symbol length, insert the next n symbols in the symbol
                 list to it's proper spot in the Huffman table. This means,
                 if symbol counts for symbols of lengths 1, 2 and 3 are 0,
                 5 and 2 respectively, the symbol list will contain 7
                 symbols, out of which the first 5 are symbols with length
                 2, and the remaining 2 are of length 3. */
                
                UInt8 code;
                m_imageFile >> std::noskipws >> code;
                
                if (m_huffmanTable[HTType][HTNumber][i].first == 0)
                {
                    while (m_huffmanTable[HTType][HTNumber][++i].first == 0);
                }   
                
                m_huffmanTable[HTType][HTNumber][i].second.push_back(code);
                syms++;
                
                if (m_huffmanTable[HTType][HTNumber][i].first == m_huffmanTable[HTType][HTNumber][i].second.size())
                    i++;
            }
            
            logFile << "Printing symbols for Huffman table (" << HTType << "," << HTNumber << ")..." << std::endl;
            
            int totalCodes = 0;
            for (auto i = 0; i < 16; ++i)
            {
                std::string codeStr = "";
                for (auto&& symbol : m_huffmanTable[HTType][HTNumber][i].second)
                {
                    std::stringstream ss;
                    ss << "0x" << std::hex << std::setfill('0') << std::setw(2) << std::setprecision(16) << (int)symbol;
                    codeStr += ss.str() + " ";
                    totalCodes++;
                }
                
                logFile << "Code length: " << i+1
                                        << ", Symbol count: " << m_huffmanTable[HTType][HTNumber][i].second.size()
                                        << ", Symbols: " << codeStr << std::endl;
            }
            
            logFile << "Total Huffman codes for Huffman table(Type:" << HTType << ",#:" << HTNumber << "): " << totalCodes << std::endl;
            
            m_huffmanTree[HTType][HTNumber].constructHuffmanTree(m_huffmanTable[HTType][HTNumber]);
            auto htree = m_huffmanTree[HTType][HTNumber].getTree();                     //Get root node of huffman tree
            logFile << "Huffman codes:-" << std::endl;
            inOrder(htree);                                                             // Output : Inorder Traveral of a huffman tree
        }
        
        logFile << "Finished parsing Huffman table segment [OK]" << std::endl;
    }
