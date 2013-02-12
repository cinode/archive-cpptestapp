#include "blob.h"
#include "utils.h"
#include "consts.h"

#include <botan/botan.h>
#include <botan/sha2_64.h>
#include <iostream>

namespace
{
    /** \brief Convert the container to botan-friendly type */
    template< typename T >
    Botan::MemoryVector< Botan::byte > toBotan( const T& data )
    {
        return Botan::MemoryVector< Botan::byte >( (const Botan::byte*)&data[0], data.size() * sizeof(data[0]) );
    }
}

void Blob::dump(std::string name) const
{
    std::cout << "======== " << name << " ==================================================================================" << std::endl;
    std::cout << std::endl;
    std::cout << "DATA: " << toHex( sourceData ) << std::endl;
    std::cout << "BID:  " << bid << std::endl;
    std::cout << "KEY:  " << key << std::endl;
    std::cout << "BLOB: " << toHex( resultData ) << std::endl;
    std::cout << std::endl;
}


void Blob::rebuildHashValidatedBlob()
{
    Botan::SHA_512 hasher;

    // Create the hash of the unencrypted buffer to be used as the encryption hey
    hasher.clear();
    hasher.update( toBotan( sourceData ) );
    Botan::SymmetricKey keyHash( hasher.final(), 32 );
    Botan::byte zeroIV[16] = {0};
    Botan::InitializationVector iv( zeroIV, sizeof(zeroIV) );

    // Encrypt the data
    Botan::Pipe pipe( Botan::get_cipher("AES-256/CFB/NoPadding", key, iv, Botan::ENCRYPTION ) );
    pipe.start_msg();
    pipe.write( toBotan( sourceData ) );
    pipe.end_msg();
    Botan::SecureVector< Botan::byte > encryptedData = pipe.read_all();

    // Generate blob id
    hasher.clear();
    hasher.update( encryptedData );

    // Get the result
    bid = toHex( hasher.final() );
    key = cipher_AES256_hexStr + toHex( keyHash.bits_of() );
    resultData.push_back( validationMethod_HashSHA512 );
    for( const auto& ch: encryptedData ) resultData.push_back(ch);
}
