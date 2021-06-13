#ifndef ARCHIVATOR_ARCHIVATOR_H
#define ARCHIVATOR_ARCHIVATOR_H
#include "Trie.h"
#include <fstream>
#include <set>
#include <utility>
#include <unordered_map>
#include <cmath>
#include <vector>
#include <algorithm>
#include <string>
#include <iostream>
#include <dir.h>

#define MY 12

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
    bool donot_delete = true;

};

class Archivator {

private:
    struct Block{
        unsigned long long info = 0;
        unsigned char count = 0;

        void Clear(){
            info = 0;
            count = 0;
        }
    };

    Settings sets;
    std::string main_Path;


public:

    Archivator(const std::string& _path, const Settings& _settings){
        main_Path = _path;
        sets = _settings;
    }

    class LZW {
    private:

        Settings setting;
        Block block;
        std::string path;

        //METHODS
        unsigned long long add_info(unsigned long long id);

        int get_info(std::ifstream&);

    public:
        explicit LZW(std::string _path, const Settings& settings){
            path = std::move(_path);
            setting = settings;
        }

        explicit LZW(const Settings& settings){
            setting = settings;
        }

        void Compress(const std::string& path_OUT);

        void Decompress (const std::string& pathIN, const std::string& pathOUT);

    };


//    ----------------------------------------------END LZW-------------------------------------------------


    class Huffman {
    private:
        struct Nodes{
            int value = 1;
            short byte = -4;
            int left_child = -2;
            int parent = -1;

            inline int right_child() const{
                return left_child + 1;
            }
        };

        const short ART = -1;
        const short END = -2;
        const short Z = -3;

/*-----------------------------------
    variables*/
        Block block;
        std::string path;
        std::vector<Nodes> root_array;
        std::unordered_map<int, int> alphabet;
//-----------------------------------


// METHODES
        unsigned long long add_info(unsigned long long id, const int& size);

        unsigned long long add_byte(unsigned char id);


        unsigned long long Calculate_index(const int& index, int& size);

        void Initial_Split(const short& byte);

        void Split_ART(const short& byte);

        void Rebuild_tree(int index);

        void Rebuild_tree_without_alphabet(int index);

        [[maybe_unused]] void Print_tree();

        [[maybe_unused]] void Check_equal();

        unsigned char Get_byte(std::ifstream& fin){
            unsigned char result = 0;
            int digits = 8;
            for (int i = 0; i < digits; ++i) {
                result = result ^ ((block.info & 1) << i);
                block.info = block.info >> 1;
                --block.count;

                if(block.info == 0 && block.count == 0){
                    fin.read((char *) &(block.info), sizeof(unsigned long long));
                    block.count = 64;
                }
            }

            return result;
        }



//_________PUBLIC___________
    public:
        explicit Huffman(const std::string& _path){
            path = _path;
        }

        Huffman() = default;

        void Compress(const std::string& path_OUT);


        void Decompress(const std::string& path_IN, const std::string& path_OUT);

    };

    void Set_path(const std::string& path){
        main_Path = path;
    }

    void Compress(const std::string& path_OUT){
        mkdir("tmp");
        LZW lzw(main_Path, sets);
        lzw.Compress("./tmp/huff.bin");
        Huffman huffman("./tmp/huff.bin");
        huffman.Compress(path_OUT);
        std::remove("./tmp/huff.bin");
        rmdir("tmp");

        if(!sets.donot_delete){
            std::remove(main_Path.c_str());
            main_Path = "";
        }
    }

    void Decompress(const std::string& path_IN, const std::string& path_OUT){
        mkdir("tmp");
        LZW lzw(sets);
        Huffman huffman;
        huffman.Decompress(path_IN, "./tmp/lzw.bin");
        lzw.Decompress("./tmp/lzw.bin", path_OUT);
        std::remove("./tmp/lzw.bin");
        rmdir("tmp");

        if(!sets.donot_delete){
            std::remove(path_IN.c_str());
        }
    }
    
};
#endif //ARCHIVATOR_ARCHIVATOR_H
