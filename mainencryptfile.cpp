#include <iostream>
#include <fstream>
#include "fileblobbuilder.h"

static const size_t buff_size = 1024;

int mainEncryptFile(int argc,char** argv)
{
    if ( argc != 4 )
    {
        std::cerr << "Usage:" << std::endl;
        std::cerr << "\t" << argv[0] << " " << argv[1] << " <filename> <output_path>" << std::endl;
        return 1;
    }

    std::ifstream in( argv[2], std::ios::binary );
    if ( !in.is_open() )
    {
        std::cerr << "Couldn't open file: " << argv[2] << std::endl;
        return 2;
    }

    char buffer[ buff_size ];
    FileBlobBuilder builder( argv[3] );

    while ( !in.eof() )
    {
        in.read( buffer, buff_size );
        auto read = in.gcount();

        builder.addContent( std::vector< char >( buffer, buffer+read ) );
    }

    Blob b = builder.finalize();

    std::cout << b.getBid() << " " << b.getKey() << std::endl;

    return 0;
}
