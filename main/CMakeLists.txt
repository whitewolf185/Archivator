cmake_minimum_required(VERSION 3.19)
project(Archivator)
set(CMAKE_CXX_STANDARD 17)

add_executable(Archivator main.cpp Trie.h LZW.h Huffman.h)
#add_executable(test test.cpp)