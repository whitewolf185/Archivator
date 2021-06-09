#include "LZW.h"
#include "Huffman.h"

int main(int argc, char* argv[]){

    LZW test("a.mp4");

//    test.Compress();

    test.Decompress("out.bin", "out.mp4");
    return 0;
}