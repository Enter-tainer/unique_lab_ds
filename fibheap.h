//
// Created by mgt on 10/29/19.
//

#ifndef LAB_FIBHEAP_H
#define LAB_FIBHEAP_H

#include <algorithm>
#include <cmath>

namespace mgt {
template<typename Key, typename Compare = std::less<Key>>
class PriorityQueue {
 private:
  struct Node {
    Key key;
    int deg{0}; // number of children
    Node *parent{nullptr}, *prev{nullptr}, *next{nullptr}, *child{nullptr};

    explicit Node(Key key) : key(key) {}
  };

  Node *min_element_{nullptr};
  size_t size_{0};
  Compare cmp_ = Compare();

  void destroy_heap(Node *rt);

 public:
  typedef Key ValueType;
  typedef Key &Reference;
  typedef const Key &ConstReference;
  typedef size_t SizeType;
  typedef Compare ValueCompare;

  ~PriorityQueue() {
    destroy_heap(min_element_);
    min_element_ = nullptr;
  }

  ConstReference top() const;

  bool empty() const;

  SizeType size() const;

  void push(const ValueType &value);

  void pop();
};

template<typename Key, typename Compare>
typename PriorityQueue<Key, Compare>::ConstReference
PriorityQueue<Key, Compare>::top() const {
  // if min_element_ == nullptr, then crash
  return min_element_->key;
}

template<typename Key, typename Compare>
bool PriorityQueue<Key, Compare>::empty() const {
  return size_ == 0;
}

template<typename Key, typename Compare>
typename PriorityQueue<Key, Compare>::SizeType
PriorityQueue<Key, Compare>::size() const {
  return size_;
}

template<typename Key, typename Compare>
void PriorityQueue<Key, Compare>::push(const ValueType &value) {
  size_++;
  if (min_element_ == nullptr) {
    min_element_ = new Node(value);
    min_element_->next = min_element_->prev = min_element_;
    // child deg & parent == nullptr
  } else {
    Node *new_node = new Node(value);
    new_node->next = min_element_->next;
    new_node->prev = min_element_;
    min_element_->next = new_node;
    new_node->next->prev = min_element_;
    if (cmp_(new_node->key, min_element_->key))
      min_element_ = new_node;
  }
}

template<typename Key, typename Compare>
void PriorityQueue<Key, Compare>::pop() {

}

template<typename Key, typename Compare>
void PriorityQueue<Key, Compare>::destroy_heap(PriorityQueue::Node *rt) {
  if (rt != nullptr) {
    Node *c = rt;
    do {
      Node *d = c;
      c = c->next;
      destroy_heap(d->child);
      delete d;
    } while (c != rt);
  }
}

}
#endif //LAB_FIBHEAP_H
