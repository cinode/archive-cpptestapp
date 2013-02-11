#include <memory>
#include <iostream>
#include <botan/sha2_64.h>
#include <botan/botan.h>
#include <QtGlobal>

#include "serializer.h"

const int         blobType_simpleStaticFile   = 0x01;
const int         blobType_splitStaticFile    = 0x02;
const int         blobType_simpleStaticDir    = 0x11;
const int         validationMethod_HashSHA512 = 0x01;
const std::string cipher_AES256_hexStr        = "01";
const size_t      simpleFileSizeLimit         = 16 * 1024*1024;

template< typename T >
std::string toHex( const T& data )
{
    Botan::Pipe hexPipe( new Botan::Hex_Encoder( Botan::Hex_Encoder::Lowercase ) );
    hexPipe.start_msg();
    hexPipe.write( data );
    hexPipe.end_msg();
    return hexPipe.read_all_as_string();
}

struct Blob
{
    Botan::SecureVector< Botan::byte > sourceData;
    Botan::SecureVector< Botan::byte > resultData;
    std::string bid;
    std::string key;

    void dump( std::string name )
    {
        std::cout << "======== " << name << " ==================================================================================" << std::endl;
        std::cout << std::endl;
        std::cout << "DATA: " << toHex( sourceData ) << std::endl;
        std::cout << "BID:  " << bid << std::endl;
        std::cout << "KEY:  " << key << std::endl;
        std::cout << "BLOB: " << toHex( resultData ) << std::endl;
        std::cout << std::endl;
    }
};

template< typename T >
Blob createBlobHash( char blobType, const T& content )
{
    Botan::SHA_512 hasher;
    Blob ret;

    // Build the unencrypted data buffer
    ret.sourceData.push_back( blobType );
    for( const auto& ch: content ) ret.sourceData.push_back(ch);

    // Create the hash of the unencrypted buffer to be used as the encryption hey
    hasher.clear();
    hasher.update( ret.sourceData );
    Botan::SymmetricKey key( hasher.final(), 32 );
    Botan::byte zeroIV[16] = {0};
    Botan::InitializationVector iv( zeroIV, sizeof(zeroIV) );

    // Encrypt the data
    Botan::Pipe pipe( Botan::get_cipher("AES-256/CFB/NoPadding", key, iv, Botan::ENCRYPTION ) );
    pipe.start_msg();
    pipe.write( ret.sourceData );
    pipe.end_msg();
    Botan::SecureVector< Botan::byte > encryptedData = pipe.read_all();

    // Generate blob id
    hasher.clear();
    hasher.update( encryptedData );

    // Get the result
    ret.bid = toHex( hasher.final() );
    ret.key = cipher_AES256_hexStr + toHex( key.bits_of() );
    ret.resultData.push_back( validationMethod_HashSHA512 );
    for( const auto& ch: encryptedData ) ret.resultData.push_back(ch);

    return ret;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
Blob createFileBlobHash( const T& content )
{
    std::vector< Blob > partials;

    for ( size_t pos = 0; pos < content.size(); pos += simpleFileSizeLimit )
    {
        size_t partSize = std::min( simpleFileSizeLimit, content.size() - pos );
        std::vector<char> partContent( content.begin()+pos, content.begin()+pos+partSize );
        Blob blob = createBlobHash( blobType_simpleStaticFile, content );
        partials.push_back( blob );
    }

    // Make sure we've got at least one blob
    if ( partials.empty() )
    {
        partials.push_back( createBlobHash( blobType_simpleStaticFile, std::string() ) );
    }

    // Don't have to split the blob if it fits in one part
    if ( partials.size() == 1 )
    {
        return partials.front();
    }

    // Create extra blob containing information about the parts
    Serializer s;

    s << content.size() << partials.size();
    for ( const auto& blob: partials ) s << blob.bid << blob.key;

    return createBlobHash( blobType_splitStaticFile, s.getData() );
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

    std::string aaa;
    for ( size_t i=0; i<simpleFileSizeLimit; i++ ) aaa.push_back('a');
    dumpFileBlobHash( aaa );
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

    createBlobHash( blobType_simpleStaticDir, s.getData() ).dump( name );
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

    createSimpleDirBlobHash( "Two entrie - empty file and 'a' file", {
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
