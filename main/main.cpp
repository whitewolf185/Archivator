#include "LZW.h"
#include "Huffman.h"

int main(int argc, char* argv[]){

    LZW test("4.jpg");
    test.Compress(".\\resultFolder\\lzw.bin");

    Huffman huff(".\\resultFolder\\lzw.bin");
//    Huffman huff("t.txt");
    huff.Compress(".\\resultFolder\\huff.bin");

    huff.Decompress(".\\resultFolder\\huff.bin", ".\\resultFolder\\dhuff.bin");
//    huff.Decompress(".\\resultFolder\\huff.bin", ".\\resultFolder\\result.txt");

    test.Decompress(".\\resultFolder\\dhuff.bin", ".\\resultFolder\\5.jpg");
    return 0;
}