#include "testvectorsdir.h"
#include "serializer.h"
#include "consts.h"
#include "blob.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

namespace
{
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

        Blob::newHashValidatedBlob( blobType_simpleStaticDir, s.getData() ).dump( name );
    }
}

void createDirTestVectors()
{
    createSimpleDirBlobHash( "Empty Dir", {} );

    createSimpleDirBlobHash( "One entry - empty file", {
    {
        "empty",
        "b4f5a7bb878c0cec9cb4bd6ae8bb175a7ea59c1a048c5ab7c119990d0041cb9cfb67c2aa9e6fada8112719777b4b80ffada80205f8ebe6981c0ade97ff3df8e5",
        "017b54b66836c1fbdd13d2441d9e1434dc62ca677fb68f5fe66a464baadecdbd00"
    }
    });

    createSimpleDirBlobHash( "Two entries - empty file and 'a' file", {
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
