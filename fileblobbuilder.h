#ifndef FILEBLOBBUILDER_H
#define FILEBLOBBUILDER_H

#include <vector>
#include "blob.h"
#include "consts.h"

class FileBlobBuilder
{
    public:

        FileBlobBuilder( std::string outPath );

        template< typename T >
        void addContent( const T& content )
        {
            static_assert( sizeof(content[0]) == 1, "content's element must be of byte size" );

            size_t pos = 0;
            size_t len = content.size();

            while ( len > 0 )
            {
                // Get the number of bytes to copy at this iteration
                size_t toCopy = std::min( simpleFileSizeLimit - m_buffer.size(), len );

                // Copy the data  to buffer
                m_buffer.reserve( m_buffer.size() + toCopy );
                std::copy( content.begin() + pos, content.begin() + (pos+toCopy), std::back_inserter(m_buffer) );

                // Create partial blob if the buffer is full
                if ( m_buffer.size() == simpleFileSizeLimit )
                {
                    finalizePartialBlob();
                }

                // Update data window
                pos += toCopy;
                len -= toCopy;
            }
        }

        Blob finalize();

    private:

        void finalizePartialBlob();

        std::string         m_outPath;   ///< \brief Output path for blobs
        std::vector< Blob > m_partials;  ///< \brief List of already generated partial blobs
        std::vector< char > m_buffer;    ///< \brief Buffer for partial content
        size_t              m_totalSize; ///< \brief Total number of bytes
};

#endif // FILEBLOBBUILDER_H
