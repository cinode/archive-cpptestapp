#ifndef SERIALIZER_H
#define SERIALIZER_H

#include <vector>
#include <cstdint>
#include <string>
#include <algorithm>

class Serializer
{
    public:

        /** \brief Serialization of standard integer values */
        void serialize( std::uint32_t value )
        {
            serializeInteger( value );
        }

        /** \brief Serialization of long values */
        void serialize( std::uint64_t value )
        {
            serializeInteger( value );
        }

        /** \brief Serialization of string */
        void serialize( const std::string& value )
        {
            serializeInteger( value.size() );
            std::copy( value.begin(), value.end(), std::back_inserter(m_container) );
        }

        /** \brief Serialization of vector */
        template< typename T >
        void serialize( const std::vector< T >& value )
        {
            serialize( value.size() );
            for( const auto& element: value ) serialize( element );
        }

        /** \brief Streaming operator */
        template< typename T >
        inline Serializer& operator<<( const T& value )
        {
            serialize( value );
            return *this;
        }

        /** \brief Get the underlying buffer */
        inline const std::vector< char > getData() const
        {
            return m_container;
        }

    private:

        template< typename T >
        inline void serializeInteger( T value )
        {
            do
            {
                char byte = value & 0x7F;
                value >>= 7;
                if ( value ) byte |= 0x80;
                putByte( byte );
            }
            while( value );
        }

        inline void putByte( char ch )
        {
            m_container.push_back( ch );
        }

        std::vector< char > m_container;
};

#endif
