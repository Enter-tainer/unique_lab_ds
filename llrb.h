#ifndef LLRB_LLRB_H
#define LLRB_LLRB_H

#include <algorithm>
#include <memory>

namespace mgt {
template<class Key, class Compare = std::less<Key>>
class Set {
 private:

  enum NodeColor {
    kBlack = 0, kRed = 1
  };

  struct Node {
    Key key;
    Node *lc, *rc;
    size_t size{0};
    NodeColor color; // the color of the parent link

    Node(Key key, NodeColor color, size_t size) : key(key), color(color), size(size) { lc = rc = nullptr; }

    Node() = default;
  };

  void destroyTree(Node *root) const {
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

  size_t size(const Node *nd) const {
    return nd == nullptr ? 0 : nd->size;
  }

  Node *rotate_left(Node *node) const {
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
    res->size = node->size;
    node->size = size(node->lc) + size(node->rc) + 1;
    return res;
  }

  Node *rotate_right(Node *node) const {
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
    res->size = node->size;
    node->size = size(node->lc) + size(node->rc) + 1;
    return res;
  }

  void color_flip(Node *node) const {
    node->color = !node->color;
    node->lc->color = !node->lc->color;
    node->rc->color = !node->rc->color;
  }

  Node *insert(Node *root, const Key &key) const;

  Node *delete_min(Node *root) const;

  Node *move_red_right(Node *root) const;

  Node *move_red_left(Node *root) const;

  Node *fix_up(Node *root) const;

  const Key &get_min(Node *root) const;

  Compare cmp_ = Compare();
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

  ~Set() {
    destroyTree(root_);
  }

  SizeType size() const;

  SizeType count(const KeyType &key) const;

  SizeType erase(const KeyType &key);

  void clear();

  void insert(const KeyType &key);

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
void Set<Key, Compare>::insert(const KeyType &key) {
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
    return new Node(key, kRed, 1);
  if (is_red(root->lc) && is_red(root->rc))
    color_flip(root);
  if (root->key == key);
  else if (cmp_(root->key, key)) // if (root->key < key)
    root->lc = insert(root->lc, key);
  else
    root->rc = insert(root->rc, key);
  fix_up(root);
  root->size = size(root->lc) + size(root->rc) + 1;
  return root;
}

template<class Key, class Compare>
typename Set<Key, Compare>::Node *
Set<Key, Compare>::delete_min(Set::Node *root) const {
  return nullptr;
}

template<class Key, class Compare>
typename Set<Key, Compare>::Node *
Set<Key, Compare>::move_red_right(Set::Node *root) const {
  color_flip(root);
  if (is_red(root->lc->lc)) // assume that root->lc != nullptr when calling this function
    root = rotate_right(root), color_flip(root);
  return root;
}

template<class Key, class Compare>
typename Set<Key, Compare>::Node *
Set<Key, Compare>::move_red_left(Set::Node *root) const {
  color_flip(root);
  if (is_red(root->rc->lc)) {
    // assume that root->rc != nullptr when calling this function
    root->rc = rotate_right(root->rc);
    root = rotate_left(root);
    color_flip(root);
  }
  return root;
}

template<class Key, class Compare>
typename Set<Key, Compare>::Node *
Set<Key, Compare>::fix_up(Set::Node *root) const {
  if (is_red(root->rc)) // fix right leaned red link
    root = rotate_left(root);
  if (is_red(root->lc) && is_red(root->lc->lc)) // fix doubly linked left leaned red link
    // if (root->lc == nullptr), then the second expr won't be evaluated
    root = rotate_right(root);
  if (is_red(root->lc) && is_red(root->rc))
    // break up 4 node
    color_flip(root);
  return root;
}

template<class Key, class Compare>
const Key &Set<Key, Compare>::get_min(Set::Node *root) const {
  Node *x = root;
  // will crash as intended when root == nullptr
  for (; x->lc != nullptr; x = x->lc);
  return x->key;
}

template<class Key, class Compare>
typename Set<Key, Compare>::SizeType
Set<Key, Compare>::size() const {
  return size(root_);
}

} // namespace mgt
#endif