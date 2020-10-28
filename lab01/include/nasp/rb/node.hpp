#pragma once

#include <memory>
#include <cstddef>

namespace nasp::rb {
  
  template<typename T>
  struct node;
  
  template<typename T>
  using node_wrapper = ::std::shared_ptr<node<T>>;
  
  template<typename T>
  struct node {
    T value;
    bool red;
    
    node_wrapper<T> parent;
    node_wrapper<T> children[2] = {};
    
    node(T const value, bool const red = false): value(value), red(red) {} 
  };
  
}