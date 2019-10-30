//
// Created by mgt on 10/28/19.
//

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file

#include <vector>
#include <algorithm>

#include <catch2/catch.hpp>

#include "llrb.h"

constexpr int n = 10000;
TEST_CASE("insert test on int", "[insert]") {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(1, 1000000);


  SECTION("insert a value twice is not an error") {
    mgt::Set<int> s;
    s.insert(1);
    s.insert(2);
    s.insert(1);
    REQUIRE(s.serialize() == std::vector<int>{1, 2});
  }

  SECTION("work with n numbers, insert sequentially") {
    mgt::Set<int> s;
    for (int i = 0; i < n; ++i)
      s.insert(i);
    std::vector<int> v = s.serialize();
    REQUIRE(v.size() == n);
    for (size_t i = 0; i < v.size(); ++i)
      REQUIRE(v[i] == i);
  }

  SECTION("work with n numbers, insert reversely") {
    mgt::Set<int> s;
    for (int i = n - 1; i >= 0; --i)
      s.insert(i);
    std::vector<int> v = s.serialize();
    REQUIRE(v.size() == n);
    for (size_t i = 0; i < v.size(); ++i)
      REQUIRE(v[i] == i);
  }

  SECTION("work with n numbers, insert randomly") {
    mgt::Set<int> s;
    std::vector<int> v;
    for (int i = 0; i < n; ++i)
      v.push_back(dis(gen));
    std::sort(v.begin(), v.end());
    auto new_end = std::unique(v.begin(), v.end());
    v.erase(new_end, v.end());
    std::shuffle(v.begin(), v.end(), gen);
    for (auto &i : v)
      s.insert(i);
    std::sort(v.begin(), v.end());
    std::vector<int> res = s.serialize();
    REQUIRE(res.size() == v.size());
    for (size_t i = 0; i < v.size(); ++i)
      REQUIRE(v[i] == res[i]);
  }
}

TEST_CASE("delete test on int", "[delete]") {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(1, 1000000);
  SECTION("delete even numbers") {
    mgt::Set<int> s;
    for (int i = 0; i < n; ++i)
      s.insert(i);
    for (int i = 0; i < n; i += 2)
      s.erase(i);
    std::vector<int> v = s.serialize();
    REQUIRE(v.size() == n / 2);
    for (size_t i = 0; i < v.size(); ++i)
      REQUIRE(v[i] == 2 * i + 1);
  }

  SECTION("delete randomly") {
    std::set<int> st;
    mgt::Set<int> s;
    while (st.size() < n)
      st.insert(dis(gen));
    std::vector<int> v(n);
    std::copy(st.begin(), st.end(), v.begin());
    for (auto &i : v)
      s.insert(i);
    std::shuffle(v.begin(), v.end(), gen);
    for (auto &i : v)
      s.erase(i);
    REQUIRE(s.empty());
  }
}

TEST_CASE("count test on int") {
  SECTION("count even numbers") {
    mgt::Set<int> s;
    for (int i = 0; i < n; ++i)
      s.insert(i);
    for (int i = 0; i < n; ++i)
      if (i < n / 2)
        REQUIRE(s.count(i * 2));
      else
        REQUIRE(s.count(i * 2) == 0);
  }
}

TEST_CASE("clear actually clear the set") {
  mgt::Set<int> s;
  s.insert(1);
  s.insert(5);
  s.insert(4);
  s.clear();
  REQUIRE(s.serialize().empty());
}

TEST_CASE("mgt::Set <=> std::set") {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, 2);
  std::uniform_int_distribution<> insert_rand(0, 1000000);
  enum Op {
    kInsert, kErase, kCount
  };
  std::vector<Op> ops;
  for (int i = 0; i < n; ++i)
    ops.push_back(Op(dis(gen)));
  mgt::Set<int> s;
  std::set<int> st;
  for (auto &i: ops) {
    switch (i) {
      case kInsert: {
        int rand_val = insert_rand(gen);
        s.insert(rand_val);
        st.insert(rand_val);
        break;
      }
      case kCount: {
        std::uniform_int_distribution<> count_rand(0, st.size());
        int rand_val = count_rand(gen);
        REQUIRE(st.count(rand_val) == s.count(rand_val));
        break;
      }
      case kErase: {
        std::uniform_int_distribution<> count_rand(0, st.size());
        auto it = st.begin();
        std::advance(it, static_cast<int>(count_rand(gen)));
        int rand_val = *it;
        st.erase(rand_val);
        s.erase(rand_val);
      }
    }
  }
  REQUIRE(st.size() == s.size());
  std::vector<int> v1(s.size()), v2 = s.serialize();
  std::copy(st.begin(), st.end(), v1.begin());
  REQUIRE(v1 == v2);
}

TEST_CASE("mgt::Set works when use a customized compare function") {
  mgt::Set<int, std::greater<>> s;
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> rand_value(0, 1000000);
  int maxvalue = 0;
  for (int i = 1; i <= n; ++i) {
    int rnd = rand_value(gen);
    maxvalue = std::max(maxvalue, rnd);
    s.insert(rnd);
  }
  REQUIRE(s.serialize()[0] == maxvalue);
}