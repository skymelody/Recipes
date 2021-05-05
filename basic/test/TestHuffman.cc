#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include "../Huffman.h"
using namespace std;
using namespace basic;

int main() {
  HuffmanFileEncoder encoder("/home/skymelody/services",
                             "/home/skymelody/lin2");
  if (!encoder.encode()) {
    std::cout << "nook" << std::endl;
  }
}
