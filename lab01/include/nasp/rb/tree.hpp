#pragma once

#include <string>
#include <cassert>
#include <iostream>
#include <functional>
#include <utility>

#include <nasp/rb/node.hpp>

namespace nasp::rb {
  
  namespace detail {
    
    template<typename T>
    inline void rotate(node_wrapper<T> const n, bool const right = true)
        noexcept {
      auto const p = n->parent;
      
      assert(p);
      assert(p->children[!right] == n);
      
      auto const tmp = n->children[right];
      n->children[right] = p;
      p->children[!right] = tmp;
      
      if (tmp) {
        tmp->parent = p;
      }
      
      if (auto const gp = p->parent; gp) {
        gp->children[gp->children[1] == p] = n;
      }
      
      n->parent = p->parent;
      p->parent = n;
    }
    
    template<typename T>
    inline void rotate_left(node_wrapper<T> const n) noexcept {
      rotate(n, false);
    }
    
    template<typename T>
    inline void rotate_right(node_wrapper<T> const n) noexcept {
      rotate(n, true);
    }
    
    template<typename T>
    node_wrapper<T> uncle(node_wrapper<T> const n) noexcept {
      assert(n);
      auto const p = n->parent;
      
      if (!p) {
        return {};
      }
      
      auto const gp = p->parent;
      
      if (!gp) {
        return {};
      }
      
      return gp->children[gp->children[0] == p];
    }
    
    // satisfies legacy forward iterator named requirement
    template<typename T>
    class tree_iter {
      using value_type   = T;
      using wrapper_type = node_wrapper<T>;
      
      wrapper_type current;
      
     public:
      tree_iter() noexcept = default;
      tree_iter(wrapper_type start) noexcept : current(start) {}
      
      tree_iter(tree_iter const&) noexcept = default;
      tree_iter& operator=(tree_iter const&) noexcept = default;
      
      tree_iter(tree_iter&&) noexcept = default;
      tree_iter& operator=(tree_iter&&) noexcept = default;
      
      bool operator==(tree_iter const& other) const noexcept {
        return current == other.current;
      }
      
      bool operator!=(tree_iter const& other) const noexcept {
        return !this->operator==(other);
      }
      
      value_type const& operator*() const noexcept {
        return current->value;
      }
      
      value_type const* operator->() const noexcept {
        return &current->value;
      }
      
      tree_iter& operator++() noexcept {
        if (!current) {
          return *this;
        }
        
        auto const& last_elem = current->value;
        
        while (!(last_elem < current->value)) {
          if (current->children[1] && last_elem < current->children[1]->value) {
            current = current->children[1];
          } else {
            current = current->parent;
          }
          
          if (!current) {
            break;
          }
          while (current->children[0] && last_elem < current->children[0]->value) {
            current = current->children[0];
          }
        }
        
        return *this;
      }
      
      tree_iter operator++(int) noexcept {
        auto old = *this;
        this->operator++();
        return old;
      }
    };
    
  }
  
  template<typename T>
  class tree {
    using value_type   = T;
    using wrapper_type = node_wrapper<value_type>;
    
    wrapper_type root;
    
    wrapper_type insert_impl(value_type const& key, wrapper_type const curr) noexcept {
      assert(curr);
      
      bool less = key < curr->value;
      bool gt   = curr->value < key;
      
      if (!less && !gt) {
        return {};
      }
      
      if (curr->children[gt]) {
        return insert_impl(key, curr->children[gt]);
      }
      
      wrapper_type n{new node{key, true}};
      curr->children[gt] = n;
      n->parent = curr;
      
      return n;
    }
    
    void insert_fix(wrapper_type n) noexcept {
      if (!n->parent) {
        n->red = false;
      } else if (!n->parent->red) {
      } else if (auto const u = detail::uncle(n); u && u->red) { // n is red but so is parent and uncle
        n->parent->red = false;
        u->red = false;
        
        auto const gp = n->parent->parent;
        gp->red = true;
        
        // we recolored grandparent, so we must ensure it does not violate rb-tree properties as well
        insert_fix(gp);
      } else { // n is red, his parent is red, but the uncle is black
        auto p = n->parent;
        auto g = p->parent;
        
        assert(g);
        
        if (n == p->children[1] && p == g->children[0]) {
          detail::rotate_left(n);
          n = n->children[0];
        } else if (n == p->children[0] && p == g->children[1]) {
          detail::rotate_right(n);
          n = n->children[1];
        }
        
        p = n->parent;
        assert(p);
        g = p->parent;
        assert(g);
        
        detail::rotate(p, p == g->children[0]);
        
        p->red = false;
        g->red = true;
      }
    }
    
    void print_impl(::std::ostream& out, wrapper_type const current)
        const noexcept {
      if (!current) {
        return;
      }
      
      out << "{";
      print_impl(out, current->children[0]);
      out << ", " << current->value << " " << (current->red ? "red" : "black")
          << ", ";
      print_impl(out, current->children[1]);
      out << "}";
    }
    
    wrapper_type get_node(value_type const& key, wrapper_type const current)
        const noexcept {
      if (!current) {
        return {};
      }
      
      bool less = key < current->value;
      bool gt   = current->value < key;
      
      if (!less && !gt) {
        return current;
      }
      
      return get_node(key, current->children[gt]);
    }
    
   public:
    tree() noexcept = default;
    
    detail::tree_iter<value_type> begin() const noexcept {
      auto jumper = root;
      if (!jumper) {
        return {};
      }
      
      while (jumper->children[0]) {
        jumper = jumper->children[0];
      }
      
      return jumper;
    }
    
    detail::tree_iter<value_type> end() const noexcept {
      return {};
    }
    
    bool insert(value_type const& key) noexcept {
      if (!root) {
        root = wrapper_type{new node{key}};
        return true;
      }
      
      auto n = insert_impl(key, root);
      
      if (!n) {
        return false;
      }
      
      insert_fix(n);
      
      while (n->parent) {
        n = n->parent;
      }
      
      root = n;
      
      return true;
    }
    
    wrapper_type get_root() noexcept {
      return root;
    }
    
    wrapper_type get(value_type const& key) noexcept {
      return get_node(key, root);
    }
    
    friend ::std::ostream& operator<<(::std::ostream& out, tree const& t)
        noexcept {
      t.print_impl(out, t.root);
      return out;
    }
  };
  
  class RedBlackTree {
    tree<char> mutable self;
    using wrapper_type = decltype(self.get_root());
    
    void PreOrderImpl(wrapper_type const current, ::std::string& result)
        const noexcept {
      if (!current) {
        return;
      }
      
      result += current->value;
      PreOrderImpl(current->children[0], result);
      PreOrderImpl(current->children[1], result);
    }
    
    void PostOrderImpl(wrapper_type const current, ::std::string& result)
        const noexcept {
      if (!current) {
        return;
      }
      
      PostOrderImpl(current->children[0], result);
      PostOrderImpl(current->children[1], result);
      result += current->value;
    }
    
   public:
    RedBlackTree() noexcept = default;
    
    void InsertElement(char key) noexcept {
      self.insert(key);
    }
    
    ::std::pair<char*, char*> getChildrenNodesValues(char* const key = nullptr)
        noexcept {
      if (!key) {
        auto const root = self.get_root();
        if (!root) {
          return {nullptr, nullptr};
        }
        return {
          root->children[0] ? &root->children[0]->value : nullptr,
          root->children[1] ? &root->children[1]->value : nullptr
        };
      }
      
      auto const node = self.get(*key);
      if (!node) {
        return {nullptr, nullptr};
      }
      
      return {
        node->children[0] ? &node->children[0]->value : nullptr,
        node->children[1] ? &node->children[1]->value : nullptr
      };
    }
    
    bool isRedNode(char* const key = nullptr) const noexcept {
      auto const node = !key ? self.get_root() : self.get(*key);
      return node ? node->red : false;
    }
    
    char* getRootNode() noexcept {
      auto const root = self.get_root();
      return root ? &root->value : nullptr;
    }
    
    ::std::string PreOrderTraversal() const noexcept {
      ::std::string result;
      PreOrderImpl(self.get_root(), result);
      return result;
    }
    
    ::std::string PostOrderTraversal() const noexcept {
      ::std::string result;
      PostOrderImpl(self.get_root(), result);
      return result;
    }
  };
  
}
