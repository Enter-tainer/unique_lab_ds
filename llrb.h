#ifndef LLRB_LLRB_H
#define LLRB_LLRB_H

#include <algorithm>
#include <memory>

namespace mgt {
template<class Key, class Compare = std::less<Key>>
class Set {
 private:
  Compare cmp_ = Compare();

  enum NodeColor {
    kBlack = 0, kRed = 1
  };

  struct Node {
    Key key;
    Node *lc, *rc;
    NodeColor color; // the color of the parent link

    Node(Key key, NodeColor color) : key(key), color(color) { lc = rc = nullptr; }

    Node() = default;
  };

  bool is_red(const Node *nd) const {
    return nd == nullptr ? kBlack : nd->color;
  }

  static Node *rotate_left(Node *node) {
    // left rotate a red link
    //          <1>                   <2>
    //        /    \\               //    \
    //       *      <2>    ==>     <1>     *
    //             /   \          /   \
    //            *     *        *     *
    Node *res = node->rc;
    node->rc = res->lc;
    res->lc = node;
    res->color = node->color;
    node->color = kRed;
    return res;
  }

  static Node *rotate_right(Node *node) {
    // right rotate a red link
    //            <1>               <2>
    //          //    \           /    \\
    //         <2>     *   ==>   *      <1>
    //        /   \                    /   \
    //       *     *                  *     *
    Node *res = node->lc;
    node->lc = res->rc;
    res->rc = node;
    res->color = node->color;
    node->color = kRed;
  }

  Node *root_;

 public:

  typedef Key KeyType;
  typedef Key ValueType;
  typedef std::size_t SizeType;
  typedef std::ptrdiff_t DifferenceType;
  typedef Compare KeyCompare;
  typedef Compare ValueCompare;
  typedef Key &Reference;
  typedef const Key &ConstReference;

  Set() = default;

  Set(Set &) = default;

  Set(Set &&) noexcept = default;

  SizeType count(const KeyType &key) const;

  SizeType erase(const KeyType &key);

  void clear();

  void insert(ConstReference key);

  bool empty() const;

};

template<class Key, class Compare>
typename Set<Key, Compare>::SizeType
Set<Key, Compare>::count(ConstReference key) const {
  Node *x = root_;
  while (x != nullptr) {
    if (key == x->key)
      return 1;
    if (cmp_(key, x->key)) // if (key < x->key)
      x = x->lc;
    else
      x = x->rc;
  }
  return 0;
}

template<class Key, class Compare>
typename Set<Key, Compare>::SizeType
Set<Key, Compare>::erase(const KeyType &key) {
  return 0; //TODO:
}

template<class Key, class Compare>
void Set<Key, Compare>::clear() {
  //TODO:
}

template<class Key, class Compare>
void Set<Key, Compare>::insert(ConstReference key) {
  //TODO:
}

template<class Key, class Compare>
bool Set<Key, Compare>::empty() const {
  return false; //TODO:
}

} // namespace mgt
#endif