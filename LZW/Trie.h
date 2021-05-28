#ifndef ARCHIVATOR_TRIE_H
#define ARCHIVATOR_TRIE_H
#include <iostream>
#include <string>
#include <map>

class Trie {
private:
    int GLOB_ID = 0;
    struct Nodes{
       char key;
       int id;
       std::map<char,Nodes*> next;
       bool leaf = true;

       Nodes(int& globID, const char& c){
           id = globID++;
           key = c;
       }

       Nodes(int& globID){
           id = globID++;
       }

    };

    Nodes* root;

//-----------iterator-----------
public:
    class Iterator{
    private:
        Nodes *node;

    public:
        Iterator(Nodes* _node){
            node = _node;
        }
        ~Iterator(){}

        //getters
        char Get_key() const{
            return node->key;
        }

        bool Is_leaf(){
            return node->leaf;
        }

        Nodes* Get_next(char c) {
            if (node->next.empty()){
                return nullptr;
            }

            auto result = node->next.find(c);
            if(result == node->next.end()){
                return nullptr;
            }
            return result->second;
        }

        std::map<char, Nodes*> Get_map(){
            return node->next;
        }

        //methods
        Nodes* Split(Iterator& cur, const std::string& str, const int& index, int& globalID){
            Nodes* next = new Nodes(globalID, str[index]);
            node->next.insert(std::pair<char,Nodes*>(str[index],next));

            return next;
        }

        void Print_node(const int& i){
            for (int j = 0; j < i; ++j) {
                std::cout << ' ';
            }
            std::cout << node->key << std::endl;
        }
    };
//-----------end of iterator-----------



public:
    Trie(){
        root = new Nodes(GLOB_ID);
    }
    //methods
    void Print(Iterator node, int i){
        if(node.Is_leaf()){
            node.Print_node(i);
            return;
        }

        for(const auto& item : node.Get_map()){
            Iterator tmp(item.second);
            node.Print_node(i);
            Print(tmp, i+1);
        }
    }

    void Insert(const std::string& str){
        Iterator node(root);
        for (int i = 0; i < str.length(); ++i) {
            auto next = node.Get_next(str[i]);
            if(next == nullptr){
                next = node.Split(node, str,i, GLOB_ID);
            }
            node = next;
        }
    }

    
    int Find(const std::string& str){
        Iterator node(root);
        for (int i = 0; i < str.length(); ++i) {
            auto next = node.Get_next(str[i]);
            if(next == nullptr){
                return false;
            }
            node = next;
        }

        return true;
    }

};


#endif //ARCHIVATOR_TRIE_H
