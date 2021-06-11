#include "LZW.h"
#include "Huffman.h"

int main(int argc, char* argv[]){

    LZW test("4.jpg");
    test.Compress("out.bin");

    Huffman huff("out.bin");
    huff.Compress("res.bin");

    huff.Decompress("res.bin", "out.bin");

//    test.Decompress("out.bin", "5.jpg");
    return 0;
}