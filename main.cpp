#include <memory>
#include <iostream>
#include <botan/sha2_64.h>
#include <botan/botan.h>
#include <QtGlobal>

const int         blobType_simpleStaticFile          = 0x01;
const std::string cipher_AES256_hexStr               = "01";
const std::string validationMethod_HashSHA512_hexStr = "01";

template< typename T >
std::string toHex( const T& data )
{
    Botan::Pipe hexPipe( new Botan::Hex_Encoder( Botan::Hex_Encoder::Lowercase ) );
    hexPipe.start_msg();
    hexPipe.write( data );
    hexPipe.end_msg();
    return hexPipe.read_all_as_string();
}

template< typename T >
void createBlobHash( char blobType, const char* blobName, const T& content )
{
    Botan::SHA_512 hasher;

    // Build the unencrypted data buffer
    Botan::SecureVector< Botan::byte > unencryptedData;
    unencryptedData.push_back( blobType );
    for( const auto& ch: content ) unencryptedData.push_back(ch);

    // Create the hash of the unencrypted buffer to be used as the encryption hey
    hasher.clear();
    hasher.update( unencryptedData );
    Botan::SymmetricKey key( hasher.final(), 32 );
    Botan::byte zeroIV[16] = {0};
    Botan::InitializationVector iv( zeroIV, sizeof(zeroIV) );

    // Encrypt the data
    Botan::Pipe pipe( Botan::get_cipher("AES-256/CFB/NoPadding", key, iv, Botan::ENCRYPTION ) );
    pipe.start_msg();
    pipe.write( unencryptedData );
    pipe.end_msg();
    Botan::SecureVector< Botan::byte > encryptedData = pipe.read_all();

    // Generate blob id
    hasher.clear();
    hasher.update( encryptedData );

    // Get the result
    std::string bidStr  = toHex( hasher.final() );
    std::string keyStr  = cipher_AES256_hexStr + toHex( key.bits_of() );
    std::string blobStr = validationMethod_HashSHA512_hexStr + toHex( encryptedData );

    std::cout << "======== " << blobName << " ==================================================================================" << std::endl;
    std::cout << std::endl;
    std::cout << "DATA: " << toHex( content ) << std::endl;
    std::cout << "BID:  " << bidStr  << std::endl;
    std::cout << "KEY:  " << keyStr  << std::endl;
    std::cout << "BLOB: " << blobStr << std::endl;
    std::cout << std::endl;
}

void createSimpleFileBlobHash( const std::string& content )
{
    std::string name = "Simple File: '" + content.substr(0,20);
    if ( content.size()>20 ) name.append("...");
    name.append("'");
    createBlobHash( blobType_simpleStaticFile, name.c_str(), content );
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