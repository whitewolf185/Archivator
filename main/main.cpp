#include "Archivator.h"

int main(int argc, char* argv[]){

    Settings settings;

    Archivator arc("4.jpg", settings);
    arc.Compress("result.bin");
    arc.Decompress("result.bin", "6.jpg");

    return 0;
}