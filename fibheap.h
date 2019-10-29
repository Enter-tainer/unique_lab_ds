//
// Created by mgt on 10/29/19.
//

#ifndef LAB_FIBHEAP_H
#define LAB_FIBHEAP_H

#include <algorithm>

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

  void consolidate();

  void insert_child(Node *root, Node *x);

  void merge_tree(Node *smaller, Node *bigger);

  void update_min_pointer();

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
    Node *mn = min_element_->next;
    new_node->next = mn;
    new_node->prev = min_element_;
    min_element_->next = new_node;
    mn->prev = new_node;
    if (cmp_(new_node->key, min_element_->key))
      min_element_ = new_node;
  }
}

template<typename Key, typename Compare>
void PriorityQueue<Key, Compare>::pop() {
  if (size_ == 1) {
    // after pop(), the heap no longer have any nodes
    // so simply remove the whole tree
    delete min_element_;
    min_element_ = nullptr;
    return;
  }
  if (min_element_->next == min_element_) {
    // the heap have only one root, but have nodes
    Node *x = min_element_->child;
    do {
      x->parent = nullptr;
      x = x->next;
    } while (x != min_element_->child);
    delete min_element_;
    min_element_ = x;
    // min_element_ also act as the head of the linked-list, so we should keep it valid
  } else {
    // the heap have more than one roots
    Node *x = min_element_->next;
    insert_child(x, min_element_);
    min_element_ = x;
  }
  // make sure min_element_ actually point to a min element
  // if not, consolidate may loop infinitely
  update_min_pointer();
  // codes above move the children of a tree x into root list, and remove tree x
  consolidate();
  // so we merge roots, thus no two roots have the same degree
  size_--;
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

template<typename Key, typename Compare>
void PriorityQueue<Key, Compare>::consolidate() {
  Node *arr[64];
  // since degree ~= log(size), 64 is greater than the max bound of degree
  std::fill(std::begin(arr), std::end(arr), nullptr);
  Node *st = min_element_, *i = st;
  // since st is the min element in the root list, it will never be merge into other roots
  // so it is safe to start & end from it
  do {
    Node *next_node = i->next; // cache next node
    Node *x = i; // cache i
    int d = x->deg;
    while (arr[d]) {
      Node *y = arr[d];
      if (x->key > y->key)
        std::swap(x, y);
      merge_tree(x, y);
      arr[d] = nullptr;
      d++;
    }
    arr[d] = x;
    i = next_node; // i = i->next
  } while (i != st);
  // end, because min pointer don't need to be updated
}

template<typename Key, typename Compare>
void PriorityQueue<Key, Compare>::insert_child(PriorityQueue::Node *root, PriorityQueue::Node *x) {
  // merge x's all children(if any) into root, clear their 'parent'
  // and then delete x
  // you should set x = nullptr after call
  if (x->child) {
    Node *t = x->child, *last_child;
    do {
      t->parent = root;
      t = t->next;
    } while (t != x->child);
    last_child = t;
    auto xp = x->prev, xn = x->next;
    // delete x, and insert [t, last_child]
    // before: xp -> x -> xn
    // after : xp -> xn -> ... -> root
    //                             |
    //                             t -> ... -> last_child -> ori_children_of_root -> ...
    xp->next = xn;
    xn->prev = xp;
    if (root->child) {
      auto cp = root->child->prev, c = root->child;
      cp->next = t;
      last_child->next = c;
      c->prev = last_child;
      t->prev = cp;
    } else {
      // if root have no child, link x's children to it
      root->child = t;
    }
  } else {
    // if x have no child, just delete x and fix linked-list
    auto xp = x->prev, xn = x->next;
    xp->next = xn;
    xn->prev = xp;
  }
  delete x;
}

template<typename Key, typename Compare>
void PriorityQueue<Key, Compare>::merge_tree(PriorityQueue::Node *smaller, PriorityQueue::Node *bigger) {
  // remove bigger from list, then make bigger one the child of smaller

  // remove bigger
  auto bp = bigger->prev, bn = bigger->next;
  bp->next = bn;
  bn->prev = bp;

  // merge as child
  if (smaller->child) {
    auto cp = smaller->child->prev, cn = smaller->child->next;
    cp->next = bigger;
    cn->prev = bigger;
    bigger->next = cn;
    bigger->prev = cp;
    bigger->parent = smaller;
  } else {
    bigger->next = bigger->prev = bigger;
    bigger->parent = smaller;
    smaller->child = bigger;
  }
  ++(smaller->deg);
}

template<typename Key, typename Compare>
void PriorityQueue<Key, Compare>::update_min_pointer() {
  Node *st = min_element_, *i = st;
  do {
    if (cmp_(i->key, min_element_->key))
      min_element_ = i;
    i = i->next;
  } while (i != st);
}

}
#endif //LAB_FIBHEAP_H
