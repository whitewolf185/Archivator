#ifndef ARCHIVATOR_HUFFMAN_H
#define ARCHIVATOR_HUFFMAN_H
#include <vector>
#include <algorithm>
#include <string>
#include <unordered_map>
#include <fstream>
#include <iostream>

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

    struct Block{
        unsigned long long info = 0;
        unsigned char count = 0;

        void Clear(){
            info = 0;
            count = 0;
        }
    };

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



//___________________________________________________PUBLIC_________________________________________________________
public:
    explicit Huffman(const std::string& _path){
        path = _path;
    }

//-----------------------------------------------------------------------------------
//                    COMPRESS
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

        Initial_Split(END);
        alphabet.insert(std::pair<int,int>(END,1));
        Rebuild_tree(2);

        unsigned long long while_counter = 0;
        unsigned char c;
        while(fin.read((char *) &(c), sizeof (unsigned char))){
            ++while_counter;

//            Print_tree();

            auto index = alphabet.find(c);
            if(index == alphabet.end()){
                //расчитываем код ART
                int size;
                auto calc_ind = Calculate_index((int)root_array.size()-1,size);
                auto tmp = add_info(calc_ind,size);
                if(tmp != 0){
                    fout.write((const char *) &(tmp), sizeof (unsigned long long));
                }

                //добавляем код элемента
                tmp = add_byte(c);
                if(tmp != 0){
                    fout.write((const char *) &(tmp), sizeof (unsigned long long));
                }

                Split_ART(c);
                alphabet.insert(std::pair<int,int>(c,root_array.size()-2));
                Rebuild_tree((int)root_array.size() - 1);
            }
            else{
                int size;
                auto calc_ind = Calculate_index(index->second,size);
                auto tmp = add_info(calc_ind,size);
                if(tmp != 0){
                    fout.write((const char *) &(tmp), sizeof (unsigned long long));
                }
                Rebuild_tree(index->second);
            }

        }

        auto index = alphabet.find(END);
        int size;
        auto calc_ind = Calculate_index(index->second,size);
        auto tmp = add_info(calc_ind,size);

        if(tmp != 0){
            fout.write((const char *) &(tmp), sizeof (unsigned long long));
            fout.write((const char *) &(block.info), sizeof (unsigned long long));
        }
        else{
            fout.write((const char *) &(block.info), sizeof (unsigned long long));
        }

        block.Clear();
        alphabet.clear();
        root_array.clear();
        fout.close();
        fin.close();

    }
//        END    COMPRESS
//-----------------------------------------------------------------------------------



//-----------------------------------------------------------------------------------
//                DECOMPRESS
    void Decompress(const std::string& path_IN, const std::string& path_OUT){
        // ----open file to Compress from----
        std::ifstream fin;
        fin.open(path_IN, std::ios::binary);

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

//        std::cout << std::endl << "Decompress" << std::endl;
        Initial_Split(END);
        Rebuild_tree_without_alphabet(2);

        int while_counter = 0;
        while(true){
            ++while_counter;
//            Print_tree();
            int iterator = 0;
            if(block.info == 0 && block.count == 0){
                fin.read((char *) &(block.info), sizeof(unsigned long long));
                block.count = 64;
            }
            while(root_array[iterator].left_child > 0){
                iterator = (block.info & 1)? (root_array[iterator].right_child()) :
                                             (root_array[iterator].left_child);
                block.info = block.info >> 1;
                --block.count;
                if(block.info == 0 && block.count == 0){
                    fin.read((char *) &(block.info), sizeof(unsigned long long));
                    block.count = 64;
                }
            }

            if(root_array[iterator].byte == Z){
                throw std::logic_error("U've made a mistake. Z can not be leaf");
            }

            if(root_array[iterator].byte == END){
                break;
            }

            if(root_array[iterator].byte == ART){
                unsigned char byte;
                byte = Get_byte(fin);
                fout.write((const char *) &(byte), sizeof(unsigned char));
                Split_ART(byte);
                Rebuild_tree_without_alphabet((int)root_array.size() - 1);
            }
            else{
                unsigned char byte;
                byte = root_array[iterator].byte;
                fout.write((const char *) &(byte), sizeof(unsigned char));
                Rebuild_tree_without_alphabet(iterator);
            }

        }

        block.Clear();
        alphabet.clear();
        root_array.clear();
        fin.close();
        fout.close();
    }
//-----------------------------------------------------------------------------------
};


#endif //ARCHIVATOR_HUFFMAN_H
