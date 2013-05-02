#include "fileblobbuilder.h"
#include "serializer.h"
#include <fstream>
#include <stdexcept>


FileBlobBuilder::FileBlobBuilder( std::string outPath )
    : m_outPath( outPath )
    , m_totalSize( 0 )
{
}

Blob FileBlobBuilder::finalize()
{
    // Finalize the blob if needed
    if ( !m_buffer.empty() || m_partials.empty() )
    {
        finalizePartialBlob();
    }

    // If there's only partial, return it as the result blob
    if ( m_partials.size() == 1 )
    {
        return m_partials.front();
    }

    // Otherwise we have to build split file blob
    Serializer s;

    s << m_totalSize << m_partials.size();
    for ( const auto& blob: m_partials ) s << blob.getBid() << blob.getKey();

    return Blob::newHashValidatedBlob( blobType_splitStaticFile, s.getData() );
}

void FileBlobBuilder::finalizePartialBlob()
{
    // Create and save the blob
    Blob blob = Blob::newHashValidatedBlob( blobType_simpleStaticFile, m_buffer );

    // Save the blob data to file
    {
        std::ofstream out( m_outPath + "/" + blob.getBid(), std::ios::binary );
        if ( !out.is_open() )
        {
            throw std::runtime_error( "Couldn't write the blob" );
        }

        out.write( &blob.getResultData()[0], blob.getResultData().size() );
    }
    blob.cutData();

    // Store blob info
    m_partials.push_back( blob );

    // Update the total size
    m_totalSize += m_buffer.size();

    // Clear the buffer
    m_buffer.clear();
}
