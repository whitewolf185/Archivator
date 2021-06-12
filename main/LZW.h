#ifndef ARCHIVATOR_LZW_H
#define ARCHIVATOR_LZW_H
#include "Trie.h"
#include <fstream>
#include <set>
#include <utility>
#include <unordered_map>
#include <cmath>

#define MY 12

/*inline int Digit_size(unsigned long long bitmask){
    for (int i = 0; i < sizeof(unsigned long long)*8; ++i) {
        if((bitmask >> i) == 1ull){
            return i+1;
        }
    }
    return 0;
}*/

class LZW {
private:
    struct Settings{
        enum Block_sizes{
            bit16,
            bit24,
            bit32,
            my
        };

        Block_sizes setted_size = bit16;

        int sizeBit = 0;
        unsigned long long max = 0;

        void Block_chooser(){
            switch (setted_size) {
                case Settings::Block_sizes::bit16:{
                    sizeBit = 16;
                    break;
                }

                case Settings::Block_sizes::bit24:{
                    sizeBit = 24;
                    break;
                }

                case Settings::Block_sizes::bit32:{
                    sizeBit = 32;
                    break;
                }

                case Settings::Block_sizes::my:{
                    sizeBit = MY;
                    break;
                }
            }
        }

        Settings(){
            Block_chooser();
            for (int i = 0; i < sizeBit; ++i) {
                max = max ^ (1ull << i);
            }
        }

        bool write_output_into_stdout = false;
        bool donot_delete = false;

    };

    struct Block{
        unsigned long long info = 0;
        char count = 0;

        void Clear(){
            info = 0;
            count = 0;
        }
    };

    Block block;
    Settings setting;
    std::string path;

    //METHODS
    unsigned long long add_info(unsigned long long id);

    int get_info(std::ifstream&);

public:
    explicit LZW(std::string _path){
        path = std::move(_path);
    }

    void Compress(const std::string& path_OUT){
        // ----open file to Compress from----
        std::ifstream fin;
        fin.open(path, std::ios::binary);

        if(!fin.is_open()){
            throw std::runtime_error("Cannot open file");
        }
        //-------


        // ----open file to Compress in----
        std::ofstream fout;
        fout.open(path_OUT, std::ios::binary);

        if(!fout.is_open()){
            throw std::runtime_error("Cannot open file");
        }
        //-------

        Trie dictionary;
        for (unsigned char c = 0; c < 255; ++c){
            dictionary.Insert(c);
        }
        dictionary.Insert(255);


        unsigned char c;
        Trie::Iterator iteration(dictionary.Get_root());
        while(fin.read((char* ) &(c), sizeof(char))) {
            auto tmp = iteration.Get_next(c);
            if(tmp == nullptr){
                int id = iteration.Get_id_node();
                auto information = add_info(id);
                if(information != 0) {
                    fout.write((const char *) &(information), sizeof(unsigned long long));
                }
                iteration.Split(c,dictionary.Get_GlobID());
                iteration = dictionary.Get_root();
                iteration = iteration.Get_next(c);
            }
            else{
                iteration = tmp;
            }

            if(dictionary.Get_GlobID() >= setting.max ){
                dictionary.Clear();
                auto information = add_info(0);
                if(information != 0){
                    fout.write((const char *) &(information), sizeof(unsigned long long));
                }
            }
        }
        int id = iteration.Get_id_node();
        auto information = add_info(id);
        if(information != 0) {
            fout.write((const char *) &(information), sizeof(unsigned long long));
        }

        information = add_info(setting.max);
        if(information != 0){
            fout.write((const char *) &(information), sizeof(unsigned long long));
            fout.write((const char *) &(block.info), sizeof(unsigned long long));
        }
        else{
            fout.write((const char *) &(block.info), sizeof(unsigned long long));
        }

        block.Clear();
        fout.close();
        fin.close();
    }

    void Decompress (const std::string& pathIN, const std::string& pathOUT){
        // ----open file to deCompress from----
        std::ifstream fin;
        fin.open(pathIN, std::ios::binary);

        if(!fin.is_open()){
            throw std::runtime_error("Cannot open file");
        }
        //-------

        // ----open file to Compress in----
        std::ofstream fout;
        fout.open(pathOUT, std::ios::binary);

        if(!fout.is_open()){
            throw std::runtime_error("Cannot open file");
        }
        //-------


        std::unordered_map<int, std::string> dictionary;
        for (unsigned char c = 0; c < 255; ++c) {
            dictionary.insert(std::pair<int,std::string> (c+1, std::string(1,c)));
        }
        dictionary.insert(std::pair<int,std::string> (256, std::string(1,255)));

        int code;
        int prev = -1;
        unsigned long long globID = 257;

        while(true) {
            //initially get data
            if(block.info == 0 && block.count == 0){
                fin.read((char *) &(block.info), sizeof(unsigned long long));
                block.count = 64;
            }

            code = get_info(fin);

            if (code == setting.max){
                break;
            }

            if(code == 0){
                prev = -1;
                globID = 257;
                for (int j = 257; j < dictionary.size(); ++j) {
                    dictionary.erase(j);
                }
                continue;
            }
            if(prev == -1){
                prev = code;
                fout << dictionary.at(code);
            }

            else if (dictionary.find(code) != dictionary.end()){
                auto tmp = dictionary.at(code);
                fout << tmp;
                dictionary.insert(std::pair<int, std::string> (globID++, dictionary.at(prev) + tmp[0]));
                prev = code;
            }

            else {
                auto tmp = dictionary.at(prev);
                tmp += tmp[0];
                fout << tmp;
                prev = globID;
                dictionary.insert(std::pair<int,std::string> (globID++, tmp));
            }
        }
        block.Clear();
        dictionary.clear();
        fout.close();
        fin.close();
    }

};


#endif //ARCHIVATOR_LZW_H
