#include "LZW.h"
#include "Huffman.h"

int main(int argc, char* argv[]){

    LZW test("5.jpg");
    test.Compress(".\\resultFolder\\lzw.bin");

    Huffman huff(".\\resultFolder\\lzw.bin");
//    Huffman huff("4.jpg");
    huff.Compress(".\\resultFolder\\huff.bin");

    huff.Decompress(".\\resultFolder\\huff.bin", ".\\resultFolder\\dhuff.bin");
//    huff.Decompress(".\\resultFolder\\huff.bin", ".\\resultFolder\\result.jpg");

    test.Decompress(".\\resultFolder\\dhuff.bin", ".\\resultFolder\\result.jpg");
    return 0;
}