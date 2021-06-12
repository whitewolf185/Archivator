#include "../LZW.h"

unsigned long long LZW::add_info(unsigned long long id){
    if(id == 0){
        unsigned long long res = 0;
        int id_digits = setting.sizeBit;
//            std::cout << id_digits << std::endl;
        block.count += id_digits;

        if(block.count >= 64){
            std::swap(res,block.info);
            block.count -= 64;
        }

        return res;
    }

    unsigned long long res = 0;
    int id_digits = setting.sizeBit;
//        std::cout << id_digits << std::endl;
    int variable = std::min(64-block.count,id_digits);


    for (int i = 0; i < variable; ++i){
        block.info = block.info ^ (((id >> i) & 1ull) << (block.count + i));
    }

    block.count = block.count + variable;

    id = id >> variable;
    res = id;
    if(res || (setting.sizeBit - variable) != 0){
        std::swap(res, block.info);
        block.count = id_digits - variable;
    }

    return res;
}

int LZW::get_info(std::ifstream& fin) {
    int result = 0;
    int digits = setting.sizeBit;
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