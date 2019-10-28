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

  void destroyTree(Node *root) {
    if (root != nullptr) {
      destroyTree(root->lc);
      destroyTree(root->rc);
      root->lc = root->rc = nullptr;
      delete root;
    }
  }

  bool is_red(const Node *nd) const {
    return nd == nullptr ? false : nd->color; // kRed == 1, kBlack == 0
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

  static void color_flip(Node *node) {
    node->color = !node->color;
    node->lc->color = !node->lc->color;
    node->rc->color = !node->rc->color;
  }

  Node *root_;

  Node *insert(Node *root, const Key &key) const;

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

  ~Set() {
    destroyTree(root_);
  }

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
  destroyTree(root_);
  root_ = nullptr;
}

template<class Key, class Compare>
void Set<Key, Compare>::insert(ConstReference key) {
  root_ = insert(root_, key);
}

template<class Key, class Compare>
bool Set<Key, Compare>::empty() const {
  return root_ == nullptr;
}

template<class Key, class Compare>
typename Set<Key, Compare>::Node *
Set<Key, Compare>::insert(Set::Node *root, const Key &key) const {
  if (root == nullptr)
    return new Node(key, kRed);
  if (is_red(root->lc) && is_red(root->rc))
    color_flip(root);
  if (root->key == key);
  else if (cmp_(root->key, key)) // if (root->key < key)
    root->lc = insert(root->lc, key);
  else
    root->rc = insert(root->rc, key);
  if (is_red(root->rc)) // fix right leaned red link
    root = rotate_left(root);
  if (is_red(root->lc) && is_red(root->lc->lc)) // fix doubly linked red link
    // if (root->lc == nullptr), then the second expr won't be evaluated
    root = rotate_right(root);
  return root;
}

} // namespace mgt
#endif