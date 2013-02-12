#ifndef UTILS_H
#define UTILS_H

#include <string>

/** \brief Convert given container to it's hex representation */
template< typename T >
std::string toHex( const T& data )
{
    static const char charTable[] = "0123456789abcdef";

    std::string ret;
    if ( data.size() > 1024 )
    {
        ret = "*** LARGE *** ";
        for ( size_t i=0; i<40; i++ )
        {
            ret.push_back( charTable[ ( data[i] >> 4 ) & 0xF ] );
            ret.push_back( charTable[ ( data[i] >> 0 ) & 0xF ] );
        }

        ret.append("...");

        for ( size_t i=data.size()-40; i<data.size(); i++ )
        {
            ret.push_back( charTable[ ( data[i] >> 4 ) & 0xF ] );
            ret.push_back( charTable[ ( data[i] >> 0 ) & 0xF ] );
        }
    }
    else
    {
        for ( const auto& val: data )
        {
            ret.push_back( charTable[ ( val >> 4 ) & 0xF ] );
            ret.push_back( charTable[ ( val >> 0 ) & 0xF ] );
        }
    }
    return ret;
}


#endif // UTILS_H
