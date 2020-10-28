#pragma once

#include <nasp/rb/node.hpp>

namespace nasp::rb::properties {
  
  template<typename T>
  inline bool is_bst(node_wrapper<T> const current,
              node_wrapper<T> const previous = {},
              bool const right_child = false) noexcept {
    if (!current) {
      return true;
    }
    
    if (previous) {
      if (right_child) {
        if (current->value <= previous->value) {
          return false;
        }
      } else if (current->value >= previous->value) {
        return false;
      }
    }
    
    return (current->children[0] ? is_bst(current->children[0], current)       : true)
        && (current->children[1] ? is_bst(current->children[1], current, true) : true);
  }
  
  template<typename T>
  inline bool every_red_has_two_blacks(node_wrapper<T> const current) noexcept {
    if (!current) {
      return true;
    }
    
    return
      (!current->red ||
        (
          (!current->children[0] || !current->children[0]->red) &&
          (!current->children[1] || !current->children[1]->red)
        )
      ) &&
      every_red_has_two_blacks(current->children[0]) &&
      every_red_has_two_blacks(current->children[1]);
  }
  
  namespace detail {
    template<typename T>
    inline ::std::int_fast32_t black_height(node_wrapper<T> const current)
        noexcept {
      if (!current) {
        return 1;
      }
      
      auto left = black_height(current->children[0]);
      
      if (left == -1) {
        return -1;
      }
      
      auto right = black_height(current->children[1]);
      
      if (right == -1 || left != right) {
        return -1;
      }
      
      return !current->red + left;
    }
  }
  
  template<typename T>
  inline bool every_black_path_equal(node_wrapper<T> const current) noexcept {
    return detail::black_height(current) != -1;
  }
  
}