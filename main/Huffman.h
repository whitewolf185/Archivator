#ifndef ARCHIVATOR_HUFFMAN_H
#define ARCHIVATOR_HUFFMAN_H
#include <vector>
#include <algorithm>

class Huffman {
private:
    struct Nodes{
        int value = 1;
        int byte;
        int left_child = -2;
        int parent = -1;

        inline int right_child(){
            return left_child + 1;
        }
    };

    const int ART = -1;
    const int END = -2;
    const int Z = -3;

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
    std::unordered_map<unsigned char, int> alphabet;
//-----------------------------------


// METHODES
    unsigned long long add_info(unsigned long long id){
        if(id == 0){
            unsigned long long res = 0;
            int id_digits = 8;
            block.count += id_digits;

            if(block.count >= 64){
                std::swap(res,block.info);
                block.count -= 64;
            }

            return res;
        }

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
    }



    unsigned long long Calculate_index(const int& index){
        if(root_array.empty()){
            return 0;
        }

        std::vector<bool> result;
        int i = index;
        while(i){
            result.push_back(!(i & 1));
            i = root_array[i].parent;
        }

        std::reverse(result.begin(), result.end());

        unsigned long long res = 0;

        for (auto && j : result) {
            res = (res | j) << 1;
        }
        res = res >> 1;

        return res;
    }

    void Initial_Split(const int& byte){
        Nodes node_ART;
        node_ART.parent = 0;
        node_ART.byte = ART;

        Nodes node;
        node.parent = 0;
        node.byte = byte;

        Nodes z;
        z.left_child = 1;
        z.byte = Z;

        root_array.push_back(z);
        root_array.push_back(node);
        root_array.push_back(node_ART);
    }

    void Split_ART(const int& byte){
        int index = (int)root_array.size()-1;

        Nodes node;
        node.parent = index;
        node.byte = byte;

        Nodes node_ART;
        node_ART.parent = index;
        node_ART.byte = ART;

        root_array[index].left_child = index + 1;
        root_array[index].byte = Z;


        root_array.push_back(node);
        root_array.push_back(node_ART);
    }

    void Rebuild_tree(int index){
        if(index == 0){
            return;
        }

        int parent = root_array[index].parent;
        ++root_array[parent].value;

        if(root_array[parent - 1].value < root_array[parent].value){
            /*Дохожу до элемента >= root_array[parent].value
             *Затем меняю местами элемент справа от найденного и мой
             * ссылки на родителей меняются
             * дети - остаются прежними*/
            index = parent - 1;
            while(index >= 0 ||
            root_array[index].value < root_array[parent].value)
            {
                --index;
            }
            ++index;

            std::swap(root_array[parent].value,root_array[index].value);
            std::swap(root_array[parent].parent,root_array[index].parent);
            std::swap(root_array[parent].byte,root_array[index].byte);
        }

        Rebuild_tree(parent);
    }


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

        unsigned char c;
        while(fin.read((char *) &(c), sizeof (unsigned char))){

            if(root_array.empty()){
                auto tmp = add_info((unsigned long long)c);
                if(tmp != 0){
                    fout << tmp;
                }
                Initial_Split(c);
                alphabet.insert(std::pair<unsigned char, int> (c,1));
                Rebuild_tree(2);
            }

            auto index = alphabet.find(c);
            if(index != alphabet.end()){
                auto tmp = add_info(Calculate_index((int)root_array.size()-1));
                if(tmp != 0){
                    fout << tmp;
                }

                tmp = add_info(Calculate_index((int)root_array.size()-1));
                if(tmp != 0){
                    fout << tmp;
                }

                Split_ART(c);
                alphabet.insert(std::pair<unsigned char,int>(c,root_array.size()-2));
                Rebuild_tree((int)root_array.size() - 1);
            }
            else{
                auto tmp = add_info(Calculate_index(index->second));
                if(tmp != 0){
                    fout << tmp;
                }
                ++root_array[index->second].value;
                Rebuild_tree(index->second);
            }

        }

    }

};


#endif //ARCHIVATOR_HUFFMAN_H
