#include <memory>
#include <iostream>
#include <botan/sha2_64.h>
#include <botan/botan.h>
#include <QtGlobal>

const int         blobType_simpleStaticFile   = 0x01;
const int         validationMethod_HashSHA512 = 0x01;
const std::string cipher_AES256_hexStr        = "01";

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

void createSimpleFileBlobHash( const std::string& content )
{
    std::string name = "Simple File: '" + content.substr(0,20);
    if ( content.size()>20 ) name.append("...");
    name.append("'");
    createBlobHash( blobType_simpleStaticFile, content ).dump( name );
}


int main(int argc, char *argv[])
{
    Botan::LibraryInitializer init;

    createSimpleFileBlobHash( "" );
    createSimpleFileBlobHash( "a" );
    createSimpleFileBlobHash( "Hello World!" );

    std::string str;
    for( char ch = 'a'; ch <= 'z'; ch++ ) str.push_back(ch);
    for( char ch = 'A'; ch <= 'Z'; ch++ ) str.push_back(ch);
    createSimpleFileBlobHash( str );

    Q_UNUSED(argc);
    Q_UNUSED(argv);

    return 0;
}
