//
// Created by mgt on 10/30/19.
//

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file

#include <vector>
#include <random>

#include <catch2/catch.hpp>
#include <queue>

#include "fibheap.h"

constexpr int n = 10000;

TEST_CASE("pq push top and pop test") {
  mgt::PriorityQueue<int> pq;
  REQUIRE(pq.empty());
  for (int i = 1; i <= n; ++i)
    pq.push(i);
  REQUIRE(pq.size() == n);
  REQUIRE(pq.top() == 1);
  for (int i = 1; i <= 20; ++i)
    pq.pop();
  REQUIRE(pq.top() == 21);
  REQUIRE(pq.size() == n - 20);
  for (int i = 21; i <= 300; ++i)
    pq.pop();
  REQUIRE(pq.top() == 301);
  REQUIRE(pq.size() == n - 300);
  while (!pq.empty())
    pq.pop();
  REQUIRE(pq.empty());
}

TEST_CASE("mgt::PriorityQueue <=> std::priority_queue") {
  enum Op {
    kPush, kPop, kTop
  };
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, 2);
  std::uniform_int_distribution<> insert_rand(0, 1000000);
  std::vector<Op> ops;
  for (int i = 0; i < n; ++i)
    ops.push_back(Op(dis(gen)));
  std::priority_queue<int, std::vector<int>, std::greater<>> sq;
  mgt::PriorityQueue<int> mq;
  for (auto &i : ops) {
    switch (i) {
      case kPush: {
        int rand_val = insert_rand(gen);
        sq.push(rand_val);
        mq.push(rand_val);
        break;
      }
      case kTop: {
        REQUIRE(sq.size() == mq.size());
        if (!mq.empty())
          REQUIRE(sq.top() == mq.top());
        break;
      }
      case kPop: {
        REQUIRE(sq.size() == mq.size());
        if (!mq.empty()) {
          mq.pop();
          sq.pop();
        }
        break;
      }
    }
  }
  REQUIRE(sq.size() == mq.size());
  while (!sq.empty()) {
    REQUIRE(sq.top() == mq.top());
    sq.pop();
    mq.pop();
  }
  REQUIRE(sq.empty());
  REQUIRE(mq.empty());
}

TEST_CASE("when compare changes, pq also works") {
  mgt::PriorityQueue<int, std::greater<>> pq;
  for (int i = 1; i <= 1000; ++i)
    pq.push(i);
  REQUIRE(pq.top() == 1000);
  pq.pop();
  REQUIRE(pq.top() == 999);
//  REQUIRE(pq.top() == 499);
}