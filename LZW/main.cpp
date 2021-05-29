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

        std::string str;
        std::cin >> str;

        std::string iteration;
        int prev = 0;
        for (int i = 0; i < str.length(); ++i) {
            iteration += str[i];
            int finder = dictionary.Find(iteration);
            if(finder >= 0){
                prev = finder;
                continue;
            }
            std::cout << prev << ' ';
            dictionary.Insert(iteration);
            iteration = str[i];
            prev = dictionary.Find(iteration);
        }

        std::cout << prev << ' ' << dictionary.Find(EOM) << std::endl;
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
                    std::cout << tmp + tmp[0];
                    dictionary.insert(std::pair<int, std::string> (globID++, tmp + tmp[0]));
                }
            }
        }
        std::cout << std::flush;
    }

    else{
        std::cerr << "Wrong command" << std::endl;
    }
}
//0 19 1 21 0 12 8 13 10 18 4 7 12 5 5 9 23 8 8 15 24 7 0 6 19 23 22 12 6 8 2 11 17 0 11 3 24 36 9 16 3 3 17 19 25 13 15 3 25 11 8 43 18 3 23 5 22 1 13 14 10 24 3 8 25 21 2 18 5 7 21 24 10 13 22 20 5 4 37 0 26
//0 19 1 21 0 12 8 13 10 18 4 7 12 5 5 9 23 8 8 15 24 7 0 6 19 23 22 12 6 8 2 11 17 0 11 3 24 36 9 16 3 3 17 19 25 13 15 3 25 11 8 43 18 3 23 5 22 1 13 14 10 24 3 8 25 21 2 18 5 7 21 24 10 13 22 20 5 4 37 0 26