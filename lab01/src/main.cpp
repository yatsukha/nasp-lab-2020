#include <iostream>
#include <fstream>
#include <string>

#include <nasp/rb/tree.hpp>

int main() {
  ::nasp::rb::tree<::std::uint_fast32_t> tree;
  
  tree.insert(12);
  tree.insert(44);
  tree.insert(1);
  tree.insert(0);
  tree.insert(13);
  
  for (auto&& v : tree) {
    ::std::cout << v << "\n";
  }
}
