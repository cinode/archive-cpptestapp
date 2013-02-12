#include <memory>
#include <iostream>
#include <botan/sha2_64.h>
#include <botan/botan.h>
#include <QtGlobal>

#include "serializer.h"
#include "consts.h"
#include "utils.h"
#include "blob.h"

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

void createFileBlobs()
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

///////////////////////////////////////////////////////////////////////////////

struct DirEntry
{
    std::string name;
    std::string bid;
    std::string key;

    inline DirEntry( std::string&& _name, std::string&& _bid, std::string&& _key )
        : name(_name)
        , bid(_bid)
        , key(_key)
    {}

    inline bool operator<( const DirEntry& other ) const
    {
        return name < other.name;
    }
};

void createSimpleDirBlobHash( const std::string& name, std::vector< DirEntry > entries )
{
    std::sort( entries.begin(), entries.end() );

    Serializer s;

    s << entries.size();
    for( const auto& entry: entries ) s << entry.name << entry.bid << entry.key;

    Blob::newHashValidatedBlob( blobType_simpleStaticDir, s.getData() ).dump( name );
}

void createDirBlobs()
{
    createSimpleDirBlobHash( "Empty Dir", {} );

    createSimpleDirBlobHash( "One entry - empty file", {
    {
        "empty",
        "b4f5a7bb878c0cec9cb4bd6ae8bb175a7ea59c1a048c5ab7c119990d0041cb9cfb67c2aa9e6fada8112719777b4b80ffada80205f8ebe6981c0ade97ff3df8e5",
        "017b54b66836c1fbdd13d2441d9e1434dc62ca677fb68f5fe66a464baadecdbd00"
    }
    });

    createSimpleDirBlobHash( "Two entries - empty file and 'a' file", {
    {
        "a",
        "c9d30a9938ecea16bed58efe5ad5b998927a56da7c8c36c1ee13292dec79aa50c5613fc90d80c37a77a5a422691d1967693a1236892e228ad95ed6fe4b505d85",
        "01504ce2f6de7e33389deb73b21f765570ad2b9f2aa8aaec8328f47b48bc3e841f"
    },
    {
        "empty",
        "b4f5a7bb878c0cec9cb4bd6ae8bb175a7ea59c1a048c5ab7c119990d0041cb9cfb67c2aa9e6fada8112719777b4b80ffada80205f8ebe6981c0ade97ff3df8e5",
        "017b54b66836c1fbdd13d2441d9e1434dc62ca677fb68f5fe66a464baadecdbd00"
    }
    });
}

int main(int argc, char *argv[])
{
    Botan::LibraryInitializer init;

    createFileBlobs();
    createDirBlobs();

    Q_UNUSED(argc);
    Q_UNUSED(argv);

    return 0;
}
