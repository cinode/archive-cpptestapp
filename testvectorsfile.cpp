#include "testvectorsfile.h"
#include "serializer.h"
#include "consts.h"
#include "blob.h"

#include <iostream>
#include <string>

namespace
{
    template< typename T >
    Blob createFileBlobHash( const T& content )
    {
        std::vector< Blob > partials;

        for ( size_t pos = 0; pos < content.size(); pos += simpleFileSizeLimit )
        {
            size_t partSize = std::min( simpleFileSizeLimit, content.size() - pos );
            std::vector<char> partContent( content.begin()+pos, content.begin()+pos+partSize );
            partials.push_back( Blob::newHashValidatedBlob( blobType_simpleStaticFile, content ) );
        }

        // Make sure we've got at least one blob
        if ( partials.empty() )
        {
            partials.push_back( Blob::newHashValidatedBlob( blobType_simpleStaticFile, std::string() ) );
        }

        // Don't have to split the blob if it fits in one part
        if ( partials.size() == 1 )
        {
            return partials.front();
        }

        std::cout << "SPLIT FILE!!!" << std::endl;

        // Create extra blob containing information about the parts
        Serializer s;

        s << content.size() << partials.size();
        for ( const auto& blob: partials ) s << blob.getBid() << blob.getKey();

        return Blob::newHashValidatedBlob( blobType_splitStaticFile, s.getData() );
    }

    void dumpFileBlobHash( const std::string& content )
    {
        std::string name = "Simple File: '" + content.substr(0,20);
        if ( content.size()>20 ) name.append("...");
        name.append("'");

        createFileBlobHash(content).dump( name );
    }
}

void createFileTestVectors()
{
    dumpFileBlobHash( "" );
    dumpFileBlobHash( "a" );
    dumpFileBlobHash( "Hello World!" );

    std::string str;
    for( char ch = 'a'; ch <= 'z'; ch++ ) str.push_back(ch);
    for( char ch = 'A'; ch <= 'Z'; ch++ ) str.push_back(ch);
    dumpFileBlobHash( str );

    /**/
    std::string aaa;
    for ( size_t i=0; i<simpleFileSizeLimit; i++ ) aaa.push_back('a');
    dumpFileBlobHash( aaa );

    /**/
}
