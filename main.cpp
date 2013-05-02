#include <QtGlobal>
#include <string>
#include <botan/botan.h>

// Forward declarations of other "main" functions
int mainTestVectors(int,char**);
int mainEncryptFile(int,char**);

int main(int argc, char *argv[])
{
    Botan::LibraryInitializer init;

    if ( argc > 1 )
    {
        std::string cmd = argv[1];
        if ( cmd == "encryptfile" || cmd == "ef" )
        {
            return mainEncryptFile( argc, argv );
        }
    }

    return mainTestVectors(argc,argv);

    Q_UNUSED(init);
    Q_UNUSED(argc);
    Q_UNUSED(argv);
}
