#ifndef ARCHIVATOR_TRIE_H
#define ARCHIVATOR_TRIE_H
#include <iostream>
#include <string>
#include <map>

class Trie {
private:
    int GLOB_ID = -1;
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

        //getters/setters
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

        int Get_id_node(){
            return node->id;
        }

        void Set_true(){
            node->leaf = true;
        }

        //methods
        Nodes* Split(const std::string& str, const int& index, int& globalID){
            Nodes* next = new Nodes(globalID, str[index]);
            node->next.insert(std::pair<char,Nodes*>(str[index],next));
            node->leaf = false;

            return next;
        }

        Nodes* Split(const char& c, int& globalID){
            Nodes* next = new Nodes(globalID, c);
            node->next.insert(std::pair<char,Nodes*>(c,next));
            node->leaf = false;

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

private:
    void Print(Iterator node, int i){
        if(node.Get_map().empty()){
            node.Print_node(i);
            return;
        }

        node.Print_node(i);
        for(const auto& item : node.Get_map()){
            Iterator tmp(item.second);
            Print(tmp, i+1);
        }
    }
    void Deleter(Nodes* node){
        if(node->next.empty()){
            delete node;
            return;
        }

        for (auto& item : node->next){
            Deleter(item.second);
        }
        delete node;
    }


public:
    Trie(){
        root = new Nodes(GLOB_ID);
    }
    ~Trie(){
        Deleter(root);
    }

    int& Get_GlobID(){
        return GLOB_ID;
    }

    //methods
    void Print(){
        Iterator node(root);
        if(node.Get_map().empty()){
            return;
        }

        for(const auto& item : node.Get_map()){
            Iterator tmp(item.second);
            Print(tmp, 0);
        }
    }


    void Insert(const std::string& str){
        Iterator node(root);
        for (int i = 0; i < str.length(); ++i) {
            auto next = node.Get_next(str[i]);
            if(next == nullptr){
                next = node.Split(str,i, GLOB_ID);
            }
            node = next;
        }
        node.Set_true();
    }

    void Insert(const char& c){
        Iterator node(root);
        auto next = node.Get_next(c);
        if(next == nullptr){
            node.Split(c,GLOB_ID);
        }
        node.Set_true();
    }

    
    int Find(const std::string& str){
        Iterator node(root);
        for (int i = 0; i < str.length(); ++i) {
            auto next = node.Get_next(str[i]);
            if(next == nullptr){
                return -1;
            }
            node = next;
        }

        return node.Get_id_node();
    }

    int Find(const char& str){
        Iterator node(root);
        auto next = node.Get_next(str);
        if(next == nullptr){
            return -1;
        }
        node = next;

        return node.Get_id_node();
    }

    Nodes* Get_root(){
        return root;
    }
};


#endif //ARCHIVATOR_TRIE_H
