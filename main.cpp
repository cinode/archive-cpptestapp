#include <QtGlobal>
#include <iostream>
#include <botan/botan.h>

#include "testvectorsfile.h"
#include "testvectorsdir.h"


int main(int argc, char *argv[])
{
    Botan::LibraryInitializer init;

    createFileTestVectors();
    createDirTestVectors();

    Q_UNUSED(init);
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    return 0;
}
