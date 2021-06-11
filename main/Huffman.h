#ifndef ARCHIVATOR_HUFFMAN_H
#define ARCHIVATOR_HUFFMAN_H
#include <vector>
#include <algorithm>

class Huffman {
private:
    struct Nodes{
        int value = 1;
        short byte;
        int left_child = -2;
        int parent = -1;

        inline int right_child(){
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
    unsigned long long add_info(unsigned long long id){
        if(id == 0){
            unsigned long long res = 0;
            int id_digits = 1;
//            std::cout << id_digits << std::endl;
            block.count += id_digits;

            if(block.count >= 64){
                std::swap(res,block.info);
                block.count -= 64;
            }

            return res;
        }

        unsigned long long res = 0;
        int id_digits = Digit_size(id);
//        std::cout << id_digits << std::endl;
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

    unsigned long long add_byte(unsigned char id){
        if(id == 0){
            unsigned long long res = 0;
            int id_digits = 8;
//            std::cout << id_digits << std::endl;
            block.count += id_digits;

            if(block.count >= 64){
                std::swap(res,block.info);
                block.count -= 64;
            }

            return res;
        }

        unsigned long long res = 0;
        int id_digits = 8;
//        std::cout << id_digits << std::endl;
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

    void Initial_Split(const short& byte){
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

    void Split_ART(const short& byte){
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
        if(index <= 0){
            return;
        }

        auto swapper = index;
        if(root_array[index - 1].value < root_array[index].value){
            /*Дохожу до элемента >= root_array[parent].value
             *Затем меняю местами элемент справа от найденного и мой
             * ссылки на родителей меняются
             * дети - остаются прежними*/
            swapper = index - 1;
            while(swapper >= 0 &&
                  root_array[swapper].value < root_array[index].value)
            {
                --swapper;
            }
            ++swapper;

//            if(index != root_array[swapper].parent && swapper != root_array[swapper].parent){
            auto lhs = alphabet.find(root_array[swapper].byte);
            auto rhs = alphabet.find(root_array[index].byte);
            if(lhs != alphabet.end()){
                lhs->second = index;
            }

            if(rhs != alphabet.end()){
                rhs->second = swapper;
            }

            std::swap(root_array[index], root_array[swapper]);
            std::swap(root_array[index].parent, root_array[swapper].parent);

            //change children's parent
            if (root_array[index].left_child > 0){
                root_array[root_array[index].left_child].parent = index;
                root_array[root_array[index].right_child()].parent = index;
            }
            if (root_array[swapper].left_child > 0){
                root_array[root_array[swapper].left_child].parent = swapper;
                root_array[root_array[swapper].right_child()].parent = swapper;
                }
//            }
        }

        int parent = root_array[swapper].parent;
        ++root_array[parent].value;

        Rebuild_tree(parent);
    }

    void Rebuild_tree_without_alphabet(int index){
        if(index <= 0){
            return;
        }

        auto swapper = index;
        if(root_array[index - 1].value < root_array[index].value){
            /*Дохожу до элемента >= root_array[parent].value
             *Затем меняю местами элемент справа от найденного и мой
             * ссылки на родителей меняются
             * дети - остаются прежними*/
            swapper = index - 1;
            while(swapper >= 0 &&
                  root_array[swapper].value < root_array[index].value)
            {
                --swapper;
            }
            ++swapper;


            std::swap(root_array[index], root_array[swapper]);
            std::swap(root_array[index].parent, root_array[swapper].parent);

            //change children's parent
            if (root_array[index].left_child > 0){
                root_array[root_array[index].left_child].parent = index;
                root_array[root_array[index].right_child()].parent = index;
            }
            if (root_array[swapper].left_child > 0){
                root_array[root_array[swapper].left_child].parent = swapper;
                root_array[root_array[swapper].right_child()].parent = swapper;
            }
        }

        int parent = root_array[swapper].parent;
        ++root_array[parent].value;

        Rebuild_tree_without_alphabet(parent);
    }

    /*void Print_tree(){
        for (int i = 0; i < root_array.size(); ++i) {
            std::cout  << root_array[i].value << ";"; //<< root_array[i].byte << ")|";
        }
        std::cout << std::endl;
    }

    void Check_equal(){
        for (int i = 0; i < root_array.size(); ++i) {
            if(root_array[i].parent == root_array[i].left_child){
                std::cout << "here comes the mistake " << i << "|";
            }
        }
        std::cout << std::endl;
    }*/

    unsigned char Get_byte(std::ifstream& fin){
        unsigned char result = 0;
        int digits = 8;
        for (int i = 0; i < digits; ++i) {
            result = result ^ ((block.info & 1) << i);
            block.info = block.info >> 1;

            if(block.info == 0){
                fin.read((char *) &(block.info), sizeof(unsigned long long));
                block.count = 0;
            }
        }

        return result;
    }


public:
    Huffman(const std::string& _path){
        path = _path;
        Initial_Split(END);
        alphabet.insert(std::pair<int,int>(END,1));
        Rebuild_tree(2);
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

        unsigned long long while_counter = 0;
        unsigned char c;
        while(fin.read((char *) &(c), sizeof (unsigned char))){
//            std::cout << while_counter << std::endl;
            ++while_counter;

//            Check_equal();

            auto index = alphabet.find(c);
            if(index == alphabet.end()){
                //расчитываем код ART
                auto tmp = add_info(Calculate_index((int)root_array.size()-1));
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
                auto tmp = add_info(Calculate_index(index->second));
                if(tmp != 0){
                    fout.write((const char *) &(tmp), sizeof (unsigned long long));
                }
                Rebuild_tree(index->second);
            }

        }

        auto index = alphabet.find(END);
        auto tmp = add_info(Calculate_index(index->second));

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

        Initial_Split(END);

        while(true){
            int iterator = 0;
            if(block.info == 0){
                fin.read((char *) &(block.info), sizeof(unsigned long long));
                block.count = 0;
            }
            while(root_array[iterator].left_child > 0){
                iterator = (block.info & 1)? (root_array[iterator].right_child()) :
                                             (root_array[iterator].left_child);
                block.info = block.info >> 1;
                if(block.info == 0){
                    fin.read((char *) &(block.info), sizeof(unsigned long long));
                    block.count = 0;
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

};


#endif //ARCHIVATOR_HUFFMAN_H
