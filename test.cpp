//
// Created by mgt on 10/28/19.
//

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file

#include <vector>
#include <algorithm>

#include <catch2/catch.hpp>

#include "llrb.h"

TEST_CASE("insert test on int", "[insert]") {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(1, 10000);

  SECTION("insert a value twice is not an error") {
    mgt::Set<int> s;
    s.insert(1);
    s.insert(2);
    s.insert(1);
    REQUIRE(s.serialize() == std::vector<int>{1, 2});
  }

  SECTION("work with 1000 numbers, insert sequentially") {
    mgt::Set<int> s;
    for (int i = 0; i < 1000; ++i)
      s.insert(i);
    std::vector<int> v = s.serialize();
    REQUIRE(v.size() == 1000);
    for (size_t i = 0; i < v.size(); ++i)
      REQUIRE(v[i] == i);
  }

  SECTION("work with 1000 numbers, insert reversely") {
    mgt::Set<int> s;
    for (int i = 999; i >= 0; --i)
      s.insert(i);
    std::vector<int> v = s.serialize();
    REQUIRE(v.size() == 1000);
    for (size_t i = 0; i < v.size(); ++i)
      REQUIRE(v[i] == i);
  }

  SECTION("work with 1000 numbers, insert randomly") {
    mgt::Set<int> s;
    std::vector<int> v;
    for (int i = 0; i < 1000; ++i)
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
  std::uniform_int_distribution<> dis(1, 10000);

  SECTION("delete even numbers") {
    mgt::Set<int> s;
    for (int i = 0; i < 1000; ++i)
      s.insert(i);
    for (int i = 0; i < 1000; i += 2)
      s.erase(i);
    std::vector<int> v = s.serialize();
    REQUIRE(v.size() == 500);
    for (size_t i = 0; i < v.size(); ++i)
      REQUIRE(v[i] == 2 * i + 1);
  }

  SECTION("delete randomly") {
    std::set<int> st;
    mgt::Set<int> s;
    while (st.size() < 1000)
      st.insert(dis(gen));
    std::vector<int> v(1000);
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
    for (int i = 0; i < 1000; ++i)
      s.insert(i);
    for (int i = 0; i < 1000; ++i)
      if (i < 500)
        REQUIRE(s.count(i * 2));
      else
        REQUIRE(s.count(i * 2) == 0);
  }
}

TEST_CASE("mgt::Set <=> std::set") {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, 2);
  std::uniform_int_distribution<> insert_rand(0, 10000);
  enum Op {
    kInsert, kErase, kCount
  };
  std::vector<Op> ops;
  for (int i = 0; i < 3000; ++i)
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