#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <iostream>
#include <vector>
#include <functional>
#include <random>

#include <nasp/rb/tree.hpp>
#include <nasp/rb/properties/properties.hpp>

template<typename T>
using container = ::std::vector<T>;

template<typename T, typename Callable>
void insert_and_test(::container<T> const& values, Callable&& test) {
  ::nasp::rb::tree<T> tree;
  for (auto&& value : values) {
    tree.insert(value);
    test(tree);
  }
}

TEST_CASE("RB-tree is a BST") {
  ::insert_and_test(
    ::container<char>{'a', 'd', 'c', 'b', 'b', 'u', 'x', 'p', 'h'},
    [](auto&& tree) {
      REQUIRE(::nasp::rb::properties::is_bst(tree.get_root()));
    }
  );
}

TEST_CASE("RB-tree has a black root node") {
  ::insert_and_test(
    ::container<char>{'a', 'd', 'c', 'b', 'b', 'u', 'x', 'p', 'h'},
    [](auto&& tree) {
      REQUIRE_FALSE(tree.get_root()->red);
    }
  );
}

TEST_CASE("In RB-tree red nodes always have two black children") {
  ::insert_and_test(
    ::container<char>{'a', 'd', 'c', 'b', 'b', 'u', 'x', 'p', 'h'},
    [](auto&& tree) {
      REQUIRE(::nasp::rb::properties::every_red_has_two_blacks(tree.get_root()));
    }
  );
}

TEST_CASE("RB-tree black height is same for every path") {
  ::insert_and_test(
    ::container<char>{'a', 'd', 'c', 'b', 'b', 'u', 'x', 'p', 'h'},
    [](auto&& tree) {
      REQUIRE(::nasp::rb::properties::every_black_path_equal(tree.get_root()));
    }
  );
}


// ugly test ahead
TEST_CASE("RB-tree stress test") {
  ::std::mt19937 mt{::std::random_device{}()};
  
  using T = ::std::uint_fast32_t;
  
  ::std::uniform_int_distribution<T> dist{0, 10'000'000};
  ::nasp::rb::tree<T> tree;
  
  ::container<T> to_insert(100'000);
  auto inserted_count = 0;
  
  for (auto i = 0; i < to_insert.size(); ++i) {
    if (tree.insert(to_insert[i] = dist(mt))) {
      ++inserted_count;
    }
    
    REQUIRE_FALSE(tree.get_root()->red);
    
    if ((i + 1) % 10'000 == 0) {
      REQUIRE(::nasp::rb::properties::is_bst(tree.get_root()));
      REQUIRE(::nasp::rb::properties::every_red_has_two_blacks(tree.get_root()));
      REQUIRE(::nasp::rb::properties::every_black_path_equal(tree.get_root()));
    }
  }
  
  for (auto&& elem : to_insert) {
    REQUIRE(tree.get(elem));
  }
  
  auto size = 0;
  
  for (auto iter = tree.begin();;) {
    ++size;
    if (auto last = *iter++; iter != tree.end()) {
      REQUIRE(last < *iter);
    } else {
      break;
    }
  }
  
  REQUIRE(size == inserted_count);
}