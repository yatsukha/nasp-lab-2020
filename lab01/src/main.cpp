#include <iostream>
#include <fstream>
#include <string>

#include <nasp/rb/tree.hpp>

int main() {
  ::nasp::rb::RedBlackTree tree;
  
  tree.InsertElement('2');
  tree.InsertElement('3');
  tree.InsertElement('1');
  tree.InsertElement('A');
  tree.InsertElement('8');
  
  ::std::cout << tree.PreOrderTraversal() << "\n";
  ::std::cout << tree.PostOrderTraversal() << "\n";
  
  ::std::cout << *tree.getRootNode() << "\n";
  ::std::cout << tree.isRedNode(tree.getRootNode()) << "\n";
  
  auto const pair = tree.getChildrenNodesValues();
  
  ::std::cout << (pair.first  ? *pair.first  : '-')
              << (pair.second ? *pair.second : '-')
              << "\n";
}
