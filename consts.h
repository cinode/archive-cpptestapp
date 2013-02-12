#ifndef CONSTS_H
#define CONSTS_H

namespace
{
    const int         blobType_simpleStaticFile   = 0x01;
    const int         blobType_splitStaticFile    = 0x02;
    const int         blobType_simpleStaticDir    = 0x11;
    const int         validationMethod_HashSHA512 = 0x01;
    const std::string cipher_AES256_hexStr        = "01";
    const size_t      simpleFileSizeLimit         = 16 * 1024*1024;
}

#endif // CONSTS_H
