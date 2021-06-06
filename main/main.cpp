#include "LZW.h"

struct Block{
    unsigned long long info = 0;
    char count = 0;

    void Clear(){
        info = 0;
        count = 0;
    }
};



int main(int argc, char* argv[]){

    LZW test("4.jpg");

    test.Compress();

    test.Decompress("out.bin", "out.jpg");
    return 0;
}