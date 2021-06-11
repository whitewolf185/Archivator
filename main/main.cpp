#include "LZW.h"
#include "Huffman.h"

int main(int argc, char* argv[]){

//    LZW test("test.txt");
//    test.Compress(".\\resultFolder\\lzw.bin");

    Huffman huff("t.txt"/*".\\resultFolder\\lzw.bin"*/);
    huff.Compress(".\\resultFolder\\huff.bin");

    huff.Decompress(".\\resultFolder\\huff.bin", ".\\resultFolder\\dhuff.txt");

//    test.Decompress(".\\resultFolder\\dhuff.bin", "dlzw.txt");
    return 0;
}