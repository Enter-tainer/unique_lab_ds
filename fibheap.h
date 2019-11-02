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

  void meld_child_to_root_list(Node *x);

  void merge_tree(Node *smaller, Node *bigger);

  void update_min_pointer();

  Node *extract_min();

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
    size_--;
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
    meld_child_to_root_list(min_element_);
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
  Node *cons[256];
  std::fill(std::begin(cons), std::end(cons), nullptr);
  Node *x, *y;
  while (min_element_) {
    x = extract_min();
    int d = x->deg;
    while (cons[d]) {
      y = cons[d];
      if (cmp_(y->key, x->key))
        std::swap(x, y);
      merge_tree(x, y);
      cons[d] = nullptr;
      ++d;
    }
    cons[d] = x;
  }

  for (auto &con : cons)
    if (con)
      con->prev = con->next = con;
  min_element_ = nullptr;
  for (auto &con : cons) {
    if (con) {
      if (min_element_ == nullptr) {
        min_element_ = con;
      } else {
        auto mn = min_element_->next;
        min_element_->next = con;
        con->next = mn;
        mn->prev = con;
        con->prev = min_element_;
        con->parent = nullptr;
        if (cmp_(con->key, min_element_->key))
          min_element_ = con;
      }
    }
  }
}

template<typename Key, typename Compare>
void PriorityQueue<Key, Compare>::meld_child_to_root_list(PriorityQueue::Node *x) {
  // merge x's all children(if any) into root list, clear their 'parent'
  // and then delete x
  if (x->child) {
    Node *t = x->child, *last_child;
    do {
      t->parent = nullptr;
      t = t->next;
    } while (t != x->child);
    last_child = t->prev;
    auto xp = x->prev, xn = x->next;
    // delete x, and insert [t, last_child]
    // before: xp -> x -> xn
    // after : xp -> t -> ... -> last_child -> xn -> ...
    xp->next = t;
    last_child->next = xn;
    xn->prev = last_child;
    t->prev = xp;
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
    auto cp = smaller->child->prev, c = smaller->child;
    cp->next = bigger;
    bigger->next = c;
    c->prev = bigger;
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

template<typename Key, typename Compare>
typename PriorityQueue<Key, Compare>::Node *
PriorityQueue<Key, Compare>::extract_min() {
  Node *p = min_element_;
  if (min_element_->next == min_element_) {
    min_element_ = nullptr;
  } else {
    auto mn = min_element_->next, mp = min_element_->prev;
    min_element_ = mn;
    mp->next = mn;
    mn->prev = mp;
  }
  p->prev = p->next = p;
  return p;
}

}
#endif //LAB_FIBHEAP_H
