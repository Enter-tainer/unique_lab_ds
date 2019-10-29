#include <iostream>
#include "llrb.h"
#include "fibheap.h"
int main() {
  mgt::PriorityQueue<int> q;
  q.push(1);
  q.push(-1);
  q.push(2);
  q.pop();
  std::cout << q.top() << std::endl;
  return 0;
}