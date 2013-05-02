#ifndef BLOB_H
#define BLOB_H

#include <vector>
#include <string>

/** \brief Universal blob representation */
class Blob
{
    public:

        /** \brief Create new hash-validated blob */
        template< typename T >
        static Blob newHashValidatedBlob( char blobType, const T& content )
        {
            static_assert( sizeof(content[0]) == 1, "content's element must be of byte size" );

            Blob ret;

            // Build the unencrypted data buffer
            ret.sourceData.resize( content.size() + 1 );
            ret.sourceData[0] = blobType;
            std::copy( content.begin(), content.end(), ret.sourceData.begin()+1 );

            // Create bid, key and encrypted data
            ret.rebuildHashValidatedBlob();

            return ret;
        }

        /** \brief Get the current key */
        inline const std::string& getKey() const { return key; }

        /** \brief Get the current bid */
        inline const std::string& getBid() const { return bid; }

        /** \brief Get the source data */
        inline const std::vector< char >& getSourceData() const { return sourceData; }

        /** \brief Get the result data */
        inline const std::vector< char >& getResultData() const { return resultData; }

        /** \brief Dump the content of a blob to the std::count */
        void dump( std::string name ) const;

    private:

        inline Blob()
        {}

        void rebuildHashValidatedBlob();

        std::vector< char > sourceData;
        std::vector< char > resultData;
        std::string bid;
        std::string key;
};

#endif // BLOB_H
