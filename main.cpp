#include <cmath>

#include "Utility.hpp"
#include "Decoder.hpp"


void printHelp()
{
    std::cout << "===========================================" << std::endl;
    std::cout << "       Temp-PEG - Simple JPEG Decoder      " << std::endl;
    std::cout << "===========================================" << std::endl;
    std::cout << "Help\n" << std::endl;
    std::cout << "<filename.jpg>                  : Decompress a JPEG image to a PPM image" << std::endl;
    std::cout << "-h                              : Print this help message and exit" << std::endl;
}

void decodeJPEG(const std::string& filename)
{
    if ( !temp_peg::utils::isValidFilename( filename ) )
    {
        std::cout << "Invalid input file name passed." << std::endl;
        return;
    }
    
    std::cout << "Decoding..." << std::endl;
    
    temp_peg::Decoder decoder;
    
    decoder.open( filename );
    if ( decoder.decodeImageFile() == temp_peg::Decoder::ResultCode::DECODE_DONE )
    {
        decoder.dumpRawData();
    }
    
    decoder.close();

    std::cout << "Generated file: " << filename.substr(0, filename.length() - 3 ) << ".ppm" << std::endl;
    std::cout << "Complete! Check log file \'kpeg.log\' for details." << std::endl;
}

int handleInput(int argc, char** argv)
{
    if ( argc < 2 )
    {
        std::cout << "No arguments provided." << std::endl;
        return EXIT_FAILURE;
    }
    
    if ( argc == 2 && (std::string)argv[1] == "-h" )
    {
        printHelp();
        return EXIT_SUCCESS;
    }
    else if ( argc == 2 )
    {
        decodeJPEG( argv[1] );
        return EXIT_SUCCESS;
    }
    
    std::cout << "Incorrect usage, use -h to view help" << std::endl;
    return EXIT_FAILURE;
}

int main( int argc, char** argv )
{
    try
    {
        logFile << "lilbKPEG - A simple JPEG library" << std::endl;
        
        return handleInput(argc, argv);
    }
    catch( std::exception& e )
    {
        std::cout << "Exceptions Occurred:-" << std::endl;
        std::cout << "What: " << e.what() << std::endl;
    }
    
    return EXIT_SUCCESS;
}