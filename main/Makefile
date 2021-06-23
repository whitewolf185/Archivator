CFLAGS=-std=c++17 -g
LDFLAGS=-lstdc++fs
DIRSOL=./cmake-build-debug/

all: main.cpp Archivator.h Trie.h ./sols/Huffman_Public.cpp ./sols/Huffman_Private.cpp ./sols/LZW_Private.cpp ./sols/LZW_Public.cpp
	g++ $(CFLAGS) main.cpp ./sols/*.cpp -o $(DIRSOL)Archivator $(LDFLAGS)

test: test.cpp
	g++ $(CFLAGS) test.cpp -o $(DIRSOL)test $(LDFLAGS)

clean:
	rm -f $(DIRSOL)main $(DIRSOL)test
