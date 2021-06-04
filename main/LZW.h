#ifndef ARCHIVATOR_LZW_H
#define ARCHIVATOR_LZW_H
#include "Trie.h"
#include <fstream>
#include <set>

class LZW {
private:
    std::set<unsigned char> alphabet;

    void compr(const std::string& str){
        std::ofstream fout;
        fout.open("compressed.bin");
        Trie dictionary;
        for (const unsigned char& c : alphabet) {
            dictionary.Insert(c);
        }

        std::string iteration;
        int prev = 0;
        for (int i = 0; i < str.length(); ++i) {
            iteration += str[i];
            int finder = dictionary.Find(iteration);
            if(finder >= 0){
                prev = finder;
                continue;
            }
            fout << prev;
            dictionary.Insert(iteration);
            iteration = str[i];
            prev = dictionary.Find(iteration);
        }
        fout.close();
    }

public:
    void Compress(const std::string& path){
        std::ifstream fin;
        fin.open(path);
        if(!fin.is_open()){
            throw std::runtime_error("Cannot open file");
        }
        unsigned char c;

        std::string block;
        bool end = false;
        while(!end) {
            for (unsigned long long i = 0; i < 536870912ll || c != EOF; ++i) {
                fin.get(c);
                if (c != EOF) {
                    block += c;
                }
                else {
                    end = true;
                }
            }
            compr(block);
        }

        fin.close();
    }

    void Decompress (const std::string& path){

    }

};


#endif //ARCHIVATOR_LZW_H
