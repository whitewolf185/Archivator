//#include "LZW.h"
#include <iostream>
#include <fstream>
int main(int argc, char* argv[]){
    std::ofstream fout;
    fout.open("test.txt");
    fout << 'a' << 'b' << 'c' << EOF << 'd' << 'e';
    fout.close();

    return 0;
}