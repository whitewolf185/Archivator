#include "../Archivator.h"

void Archivator::LZW::Compress(const std::string& path_OUT){
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



void Archivator::LZW::Decompress (const std::string& pathIN, const std::string& pathOUT){
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