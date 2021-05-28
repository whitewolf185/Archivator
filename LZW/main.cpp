#include "Trie.h"

int main() {
    Trie tree;
    std::string str;

    int count;
    std::cin >> count;
    for (int i = 0; i < count; ++i) {
        std::cin >> str;
        tree.Insert(str);
        tree.Print();
    }

    std::cout << "Enter ur string to find" << std::endl;
    std::cin >> str;
    std::cout << tree.Find(str) << std::endl;

}
