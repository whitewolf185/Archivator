#include "LZW.h"
#include "Huffman.h"

int main(int argc, char* argv[]){

    LZW test("4.jpeg");
    Huffman huff("out.bin");
    test.Compress();
    huff.Compress("res.bin");

//    test.Decompress("out.bin", "out.mp4");
    return 0;
}