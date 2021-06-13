#include "../Archivator.h"

void Archivator::Huffman::Compress(const std::string& path_OUT){
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

//        unsigned long long while_counter = 0;
    unsigned char c;
    while(fin.read((char *) &(c), sizeof (unsigned char))){
//            ++while_counter;

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









void Archivator::Huffman::Decompress(const std::string& path_IN, const std::string& path_OUT){
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

//        int while_counter = 0;
    while(true){
//            ++while_counter;
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