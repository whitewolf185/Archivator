#include "../Archivator.h"

unsigned long long Archivator::Huffman::add_info(unsigned long long id, const int& size){
    if(id == 0){
        unsigned long long res = 0;
        int id_digits = size;
//            std::cout << id_digits << std::endl;
        block.count += id_digits;

        if(block.count >= 64){
            std::swap(res,block.info);
            block.count -= 64;
        }

        return res;
    }

    unsigned long long res = 0;
    int id_digits = size;
//        std::cout << id_digits << std::endl;
    int variable = std::min(64-block.count,id_digits);


    for (int i = 0; i < variable; ++i){
        block.info = block.info ^ (((id >> i) & 1ull) << (block.count + i));
    }

    block.count = block.count + variable;

    id = id >> variable;
    res = id;
    if(res || (size - variable) != 0){
        std::swap(res, block.info);
        block.count = id_digits - variable;
    }

    return res;
}

unsigned long long Archivator::Huffman::add_byte(unsigned char id){
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
    if(res || (id_digits - variable) != 0){
        std::swap(res, block.info);
        block.count = id_digits - variable;
    }

    return res;
}

unsigned long long Archivator::Huffman::Calculate_index(const int& index, int& size){
    if(root_array.empty()){
        return 0;
    }

    std::vector<bool> result;
    int i = index;
    while(i){
        result.push_back(!(i & 1));
        i = root_array[i].parent;
    }

    size = (int)result.size();

    unsigned long long res = 0;

    for (auto && j : result) {
        res = (res | j) << 1;
    }

    res = res >> 1;

    return res;
}



void Archivator::Huffman::Initial_Split(const short& byte){
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

void Archivator::Huffman::Split_ART(const short& byte){
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

void Archivator::Huffman::Rebuild_tree(int index){
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

void Archivator::Huffman::Rebuild_tree_without_alphabet(int index){
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

[[maybe_unused]] void Archivator::Huffman::Print_tree(){
    for (auto & i : root_array) {
        std::cout  << i.value << ";"; //<< root_array[i].byte << ")|";
    }
    std::cout << std::endl;
}

[[maybe_unused]] void Archivator::Huffman::Check_equal(){
    for (int i = 0; i < root_array.size(); ++i) {
        if(root_array[i].parent == root_array[i].left_child){
            std::cout << "here comes the mistake " << i << "|";
        }
    }
    std::cout << std::endl;
}
