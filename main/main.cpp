#include "LZW.h"
#include "Huffman.h"

int main(int argc, char* argv[]){

    LZW test("a.mp4");
    Huffman huff("out.bin");
    test.Compress();
    huff.Compress("res.bin");

//    test.Decompress("out.bin", "out.mp4");
    return 0;
}