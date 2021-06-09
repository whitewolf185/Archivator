#ifndef ARCHIVATOR_LZW_H
#define ARCHIVATOR_LZW_H
#include "Trie.h"
#include <fstream>
#include <set>
#include <utility>
#include <unordered_map>
#include <cmath>

inline int Digit_size(unsigned long long bitmask){
    for (int i = 0; i < sizeof(unsigned long long)*8; ++i) {
        if((bitmask >> i) == 1ull){
            return i+1;
        }
    }
    return 0;
}

class LZW {
private:
    struct Settings{
        enum Block_sizes{
            mb512,
            gb,
            gb2,
            Full
        };

        Block_sizes block_size = mb512;
        bool write_output_into_stdout = false;
        bool donot_delete = false;

    };

    /*struct Block{
        unsigned long long info = 0;
        char count = 0;

        void Clear(){
            info = 0;
            count = 0;
        }
    };*/

//    Block block;
    Settings setting;
    std::string path;


    /*unsigned long long add_info(unsigned long long id){
        unsigned long long res = 0;
        int id_digits = Digit_size(id);
        int variable = std::min(64-block.count,id_digits);


        for (int i = 0; i < variable; ++i){
            block.info = block.info ^ (((id >> i) & 1ull) << (block.count + i));
        }

        block.count = block.count + variable;

        id = id >> variable;
        res = id;
        if(res){
            std::swap(res, block.info);
            block.count = id_digits - variable;
        }

        return res;
    }*/

    unsigned long long Block_chooser(const Settings::Block_sizes& block){
        long long result = 0;

        switch (block) {
            case Settings::Block_sizes::mb512:{
                result = 536870912;
                break;
            }

            case Settings::Block_sizes::gb2:{
                result = 2147483648;
                break;
            }

            case Settings::Block_sizes::gb:{
                result = 1073741824;
                break;
            }

            case Settings::Block_sizes::Full:{
                result = 0;
                break;
            }
        }

        return result;
    }




public:
    explicit LZW(std::string _path){
        path = std::move(_path);
    }

    void Compress(){
        // ----open file to Compress from----
        std::ifstream fin;
        fin.open(path, std::ios::binary);

        if(!fin.is_open()){
            throw std::runtime_error("Cannot open file");
        }
        //-------


        // ----open file to Compress in----
        std::ofstream fout;
        fout.open("out.bin", std::ios::binary);

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
        bool end = false;
        auto counter = Block_chooser(setting.block_size);
        while(!end) {
            Trie::Iterator iteration(dictionary.Get_root());
            unsigned long long i;
            for (i = 0; fin.read((char* ) &(c), sizeof(char)) && i < counter; ++i) {
                auto tmp = iteration.Get_next(c);
                if(tmp == nullptr){
                    int id = iteration.Get_id_node();
                    fout.write((const char *) &(id), sizeof(int));
                    iteration.Split(c,dictionary.Get_GlobID());
                    iteration = dictionary.Get_root();
                    iteration = iteration.Get_next(c);
                }
                else{
                    iteration = tmp;
                }
            }

            int id = iteration.Get_id_node();
            fout.write((const char *) &(id), sizeof(int));

            if(i < counter){
                end = true;
            }
            else{
                dictionary.Clear();
                int tmp = 0;
                fout.write((const char *) &(tmp), sizeof(int));
            }
        }
//        fout.write((const char *) &(block.info), sizeof(unsigned long long));
//        block.Clear();

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

        bool end = false;
        auto counter = Block_chooser(setting.block_size);
        while(!end) {
            unsigned long long i;
            for (i = 0; fin.read(reinterpret_cast<char *>(&code), sizeof(int)) && i < counter; ++i) {
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

                else{
                    auto tmp = dictionary.at(prev);
                    tmp += tmp[0];
                    fout << tmp;
                    prev = globID;
                    dictionary.insert(std::pair<int,std::string> (globID++, tmp));
                }
            }

            if(i < counter){
                end = true;
            }
        }
        fout.close();
        fin.close();
    }

};


#endif //ARCHIVATOR_LZW_H
