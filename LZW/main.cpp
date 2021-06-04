#include "Trie.h"
#include <unordered_map>

const char EOM = '$';

int main() {
    std::string command;
    std::cin >> command;

    if(command == "compress"){
        Trie dictionary;
        for (char c = 'a'; c <= 'z'; ++c) {
            dictionary.Insert(c);
        }
        dictionary.Insert(EOM);
        Trie::Iterator iteration(dictionary.Get_root());
        char str;
        while(std::cin >> str) {
            auto tmp = iteration.Get_next(str);
            if(tmp == nullptr){
                std::cout << iteration.Get_id_node() << ' ';
                iteration.Split(str,dictionary.Get_GlobID());
                iteration = dictionary.Get_root();
                iteration = iteration.Get_next(str);
            }
            else{
                iteration = tmp;
            }
        }
        std::cout << iteration.Get_id_node() << ' ' << dictionary.Find(EOM) << std::endl;
    }

    else if(command == "decompress"){
        std::unordered_map<int, std::string> dictionary;
        for (char c = 'a'; c <= 'z'; ++c) {
            dictionary.insert(std::pair<int, std::string> (c - 'a', std::string(1,c)));
        }
        dictionary.insert(std::pair<int,std::string> (26, std::string(1,EOM)));

        int code;
        int prev = -1;
        int globID = 27;
        while(std::cin >> code){
            if(code == 26){
                break;
            }

            if(prev == -1){
                prev = code;
                std::cout << dictionary.at(code);
            }

            else{
                if(dictionary.find(code) != dictionary.end()){
                    auto tmp = dictionary.at(code);
                    std::cout << tmp;
                    dictionary.insert(std::pair<int,std::string>(globID++,dictionary.at(prev) + tmp[0]));
                    prev = code;
                }

                else{
                    auto tmp = dictionary.at(prev);
                    tmp += tmp[0];
                    std::cout << tmp;
                    prev = globID;
                    dictionary.insert(std::pair<int, std::string> (globID++, tmp));
                }
            }
        }
        std::cout << std::flush;
    }

    else{
        std::cerr << "Wrong command" << std::endl;
    }
}