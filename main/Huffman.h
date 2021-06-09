#ifndef ARCHIVATOR_HUFFMAN_H
#define ARCHIVATOR_HUFFMAN_H
#include <vector>

class Huffman {
private:
    struct Nodes{
        int value;
        char byte;
        Nodes* left_son;
        Nodes* right_son;
        bool is_ART = true;
    };

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

    std::string path;
    std::vector<Nodes> root_array;

public:
    explicit Huffman(const std::string& _path){
        path = _path;
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



    }

};


#endif //ARCHIVATOR_HUFFMAN_H
