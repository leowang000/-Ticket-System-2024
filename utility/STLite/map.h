#ifndef TICKET_SYSTEM_MAP_H
#define TICKET_SYSTEM_MAP_H

#include <iostream>

#include "algorithm.h"
#include "Pair.h"

namespace bubble {

template<class Key, class T, class KeyCompare = Less<Key>>
class Map {
 private:
  enum Colour {
    kRed, kBlack
  };

  struct RBTreeNode {
    Colour colour_;
    Pair<const Key, T> data_;
    RBTreeNode *parent_, *left_, *right_;

    explicit RBTreeNode(const Pair<const Key, T> &data, Colour colour, RBTreeNode *parent = nullptr,
                        RBTreeNode *left = nullptr, RBTreeNode *right = nullptr) :
        data_(data), colour_(colour), parent_(parent), left_(left), right_(right) {}
  };

  static void ClearTree(RBTreeNode *root) {
    if (root == nullptr) {
      return;
    }
    ClearTree(root->left_);
    ClearTree(root->right_);
    delete root;
  }

  static RBTreeNode *CopyTree(const RBTreeNode *other_root) {
    if (other_root == nullptr) {
      return nullptr;
    }
    RBTreeNode *now_root = new RBTreeNode(other_root->data_, other_root->colour_, nullptr, CopyTree(other_root->left_),
                                          CopyTree(other_root->right_));
    if (now_root->left_ != nullptr) {
      now_root->left_->parent_ = now_root;
    }
    if (now_root->right_ != nullptr) {
      now_root->right_->parent_ = now_root;
    }
    return now_root;
  }

  template<class PtrType>
  static PtrType GetMinimum(PtrType root) {
    PtrType res = root;
    while (res->left_ != nullptr) {
      res = res->left_;
    }
    return res;
  }

  template<class PtrType>
  static PtrType GetMaximum(PtrType root) {
    PtrType res = root;
    while (res->right_ != nullptr) {
      res = res->right_;
    }
    return res;
  }

  template<class PtrType>
  static PtrType FindNext(PtrType node) {
    if (node->right_ != nullptr) {
      return GetMinimum(node->right_);
    }
    while (node->parent_ != nullptr && node->parent_->right_ == node) {
      node = node->parent_;
    }
    return node->parent_;
  }

  template<class PtrType>
  static PtrType FindPrev(PtrType node) {
    if (node->left_ != nullptr) {
      return GetMaximum(node->left_);
    }
    while (node->parent_ != nullptr && node->parent_->left_ == node) {
      node = node->parent_;
    }
    return node->parent_;
  }

  void LL(RBTreeNode *&g) {
    RBTreeNode *g_par = g->parent_, *p = g->left_, *p_r = p->right_;
    p->right_ = g;
    g->parent_ = p;
    p->parent_ = g_par;
    g->left_ = p_r;
    if (p_r != nullptr) {
      p_r->parent_ = g;
    }
    if (g_par != nullptr) {
      if (g_par->left_ == g) {
        g_par->left_ = p;
      }
      else {
        g_par->right_ = p;
      }
    }
    else {
      root_ = p;
    }
  }

  void LLb(RBTreeNode *g) {
    LL(g);
    g->colour_ = kRed;
    g->parent_->colour_ = kBlack;
  }

  void RR(RBTreeNode *g) {
    RBTreeNode *g_par = g->parent_, *p = g->right_, *p_l = p->left_;
    p->left_ = g;
    g->parent_ = p;
    p->parent_ = g_par;
    g->right_ = p_l;
    if (p_l != nullptr) {
      p_l->parent_ = g;
    }
    if (g_par != nullptr) {
      if (g_par->left_ == g) {
        g_par->left_ = p;
      }
      else {
        g_par->right_ = p;
      }
    }
    else {
      root_ = p;
    }
  }

  void RRb(RBTreeNode *g) {
    RR(g);
    g->colour_ = kRed;
    g->parent_->colour_ = kBlack;
  }

  void LR(RBTreeNode *g) {
    RBTreeNode *g_par = g->parent_, *p = g->left_, *x = p->right_, *x_l = x->left_, *x_r = x->right_;
    g->parent_ = x;
    g->left_ = x_r;
    p->parent_ = x;
    p->right_ = x_l;
    x->parent_ = g_par;
    x->left_ = p;
    x->right_ = g;
    if (x_l != nullptr) {
      x_l->parent_ = p;
    }
    if (x_r != nullptr) {
      x_r->parent_ = g;
    }
    if (g_par != nullptr) {
      if (g_par->left_ == g) {
        g_par->left_ = x;
      }
      else {
        g_par->right_ = x;
      }
    }
    else {
      root_ = x;
    }
  }

  void LRb(RBTreeNode *g) {
    LR(g);
    g->colour_ = kRed;
    g->parent_->colour_ = kBlack;
  }

  void RL(RBTreeNode *g) {
    RBTreeNode *g_par = g->parent_, *p = g->right_, *x = p->left_, *x_l = x->left_, *x_r = x->right_;
    g->parent_ = x;
    g->right_ = x_l;
    p->parent_ = x;
    p->left_ = x_r;
    x->parent_ = g_par;
    x->left_ = g;
    x->right_ = p;
    if (x_l != nullptr) {
      x_l->parent_ = g;
    }
    if (x_r != nullptr) {
      x_r->parent_ = p;
    }
    if (g_par != nullptr) {
      if (g_par->left_ == g) {
        g_par->left_ = x;
      }
      else {
        g_par->right_ = x;
      }
    }
    else {
      root_ = x;
    }
  }

  void RLb(RBTreeNode *g) {
    RL(g);
    g->colour_ = kRed;
    g->parent_->colour_ = kBlack;
  }

  Pair<RBTreeNode *, bool> InsertNode(const Pair<const Key, T> &value) {
    if (root_ == nullptr) {
      root_ = new RBTreeNode(value, kBlack);
      return {root_, true};
    }
    RBTreeNode *now = root_, *res;
    while (true) {
      bool is_smaller = KeyCompare()(value.first, now->data_.first);
      bool is_bigger = KeyCompare()(now->data_.first, value.first);
      if (!is_smaller && !is_bigger) {
        return {now, false};
      }
      if (is_smaller && now->left_ == nullptr) {
        now->left_ = new RBTreeNode(value, kRed, now);
        res = now->left_;
        if (now->colour_ == kBlack) {
          break;
        }
        if (now->parent_->left_ == now) {
          LLb(now->parent_);
        }
        else {
          RLb(now->parent_);
        }
        break;
      }
      if (is_bigger && now->right_ == nullptr) {
        now->right_ = new RBTreeNode(value, kRed, now);
        res = now->right_;
        if (now->colour_ == kBlack) {
          break;
        }
        if (now->parent_->left_ == now) {
          LRb(now->parent_);
        }
        else {
          RRb(now->parent_);
        }
        break;
      }
      if (now->colour_ == kBlack && now->left_ != nullptr && now->left_->colour_ == kRed &&
          now->right_ != nullptr && now->right_->colour_ == kRed) {
        RBTreeNode *now_par = now->parent_;
        now->colour_ = kRed;
        if (now->left_ != nullptr) {
          now->left_->colour_ = kBlack;
        }
        if (now->right_ != nullptr) {
          now->right_->colour_ = kBlack;
        }
        if (now_par != nullptr && now_par->colour_ == kRed) {
          if (now_par->left_ == now) {
            if (now_par->parent_->left_ == now_par) {
              LLb(now_par->parent_);
            }
            else {
              RLb(now_par->parent_);
            }
          }
          else {
            if (now_par->parent_->left_ == now_par) {
              LRb(now_par->parent_);
            }
            else {
              RRb(now_par->parent_);
            }
          }
        }
      }
      now = (is_smaller ? now->left_ : now->right_);
    }
    root_->colour_ = kBlack;
    return {res, true};
  }

  void EraseNode(RBTreeNode *node) {
    if (size_ == 1) {
      root_ = nullptr;
      delete node;
      return;
    }
    RBTreeNode *now = root_;
    bool find_substitution = false;
    while (true) {
      while (true) {
        if (now->colour_ == kRed) {
          break;
        }
        RBTreeNode *&par = now->parent_;
        if ((now->left_ == nullptr || now->left_->colour_ == kBlack) &&
            (now->right_ == nullptr || now->right_->colour_ == kBlack)) {
          if (par == nullptr) {
            now->colour_ = kRed;
            break;
          }
          bool is_now_left_son = (par->left_ == now);
          RBTreeNode *sib = (is_now_left_son ? par->right_ : par->left_);
          if (sib == nullptr || ((sib->left_ == nullptr || sib->left_->colour_ == kBlack) &&
                                 (sib->right_ == nullptr || sib->right_->colour_ == kBlack))) {
            par->colour_ = kBlack;
            now->colour_ = kRed;
            if (sib != nullptr) {
              sib->colour_ = kRed;
            }
            break;
          }
          if (is_now_left_son) {
            if (sib->right_ != nullptr && sib->right_->colour_ == kRed) {
              par->colour_ = kBlack;
              now->colour_ = kRed;
              sib->colour_ = kRed;
              sib->right_->colour_ = kBlack;
              RR(par);
              break;
            }
            else {
              par->colour_ = kBlack;
              now->colour_ = kRed;
              RL(par);
              break;
            }
          }
          else {
            if (sib->left_ != nullptr && sib->left_->colour_ == kRed) {
              par->colour_ = kBlack;
              now->colour_ = kRed;
              sib->colour_ = kRed;
              sib->left_->colour_ = kBlack;
              LL(par);
              break;
            }
            else {
              par->colour_ = kBlack;
              now->colour_ = kRed;
              LR(par);
              break;
            }
          }
        }
        if ((!find_substitution && now == node) || (find_substitution && now->left_ == nullptr)) {
          if (now->left_ != nullptr && now->right_ != nullptr) {
            if (now->right_->colour_ == kRed) {
              break;
            }
            LLb(now);
            break;
          }
          if (now->left_ == nullptr) {
            RRb(now);
          }
          else {
            LLb(now);
          }
          break;
        }
        bool is_smaller = (find_substitution || KeyCompare()(node->data_.first, now->data_.first));
        now = (is_smaller ? now->left_ : now->right_);
        if (now->colour_ == kRed) {
          break;
        }
        if (is_smaller) {
          RRb(now->parent_);
        }
        else {
          LLb(now->parent_);
        }
      }
      RBTreeNode *&par = now->parent_;
      bool is_now_left_son = (par != nullptr && par->left_ == now);
      if (now == node) {
        if (now->left_ == nullptr && now->right_ == nullptr) {
          if (par == nullptr) {
            break;
          }
          if (is_now_left_son) {
            par->left_ = nullptr;
          }
          else {
            par->right_ = nullptr;
          }
          break;
        }
        if (now->left_ == nullptr) {
          now->right_->parent_ = par;
          if (par == nullptr) {
            break;
          }
          if (is_now_left_son) {
            par->left_ = now->right_;
          }
          else {
            par->right_ = now->right_;
          }
          break;
        }
        if (now->right_ == nullptr) {
          now->left_->parent_ = par;
          if (par == nullptr) {
            break;
          }
          if (is_now_left_son) {
            par->left_ = now->left_;
          }
          else {
            par->right_ = now->left_;
          }
          break;
        }
        find_substitution = true;
        now = now->right_;
        continue;
      }
      if (find_substitution && now->left_ == nullptr) {
        if (node->left_ != nullptr) {
          node->left_->parent_ = now;
        }
        if (now == node->right_) {
          if (node->parent_ != nullptr) {
            if (node->parent_->left_ == node) {
              node->parent_->left_ = now;
            }
            else {
              node->parent_->right_ = now;
            }
          }
          else {
            root_ = now;
          }
          par = node->parent_;
          now->left_ = node->left_;
          now->colour_ = node->colour_;
          break;
        }
        par->left_ = nullptr;
        node->right_->parent_ = now;
        if (node->parent_ != nullptr) {
          if (node->parent_->left_ == node) {
            node->parent_->left_ = now;
          }
          else {
            node->parent_->right_ = now;
          }
        }
        else {
          root_ = now;
        }
        par = node->parent_;
        now->left_ = node->left_;
        now->right_ = node->right_;
        now->colour_ = node->colour_;
        break;
      }
      now = (find_substitution || KeyCompare()(node->data_.first, now->data_.first) ? now->left_ : now->right_);
    }
    root_->colour_ = kBlack;
    delete node;
  }

  RBTreeNode *FindNode(const Key &key) const {
    RBTreeNode *now = root_;
    while (now != nullptr) {
      bool is_smaller = KeyCompare()(key, now->data_.first), is_bigger = KeyCompare()(now->data_.first, key);
      if (!is_smaller && !is_bigger) {
        return now;
      }
      now = (is_smaller ? now->left_ : now->right_);
    }
    return nullptr;
  }

  RBTreeNode *root_, *min_node_, *max_node_;
  size_t size_;

 public:
  typedef Pair<const Key, T> value_type;

  class const_iterator;

  class iterator {
   private:
    friend Map<Key, T, KeyCompare>;

    iterator(RBTreeNode *node, Map<Key, T, KeyCompare> *map_ptr) : node_(node), map_ptr_(map_ptr) {}

    RBTreeNode *node_;
    const Map<Key, T, KeyCompare> *map_ptr_;
    
   public:
    iterator() : node_(nullptr), map_ptr_(nullptr) {}

    iterator(const iterator &other) : node_(other.node_), map_ptr_(other.map_ptr_) {}
    
    iterator operator++(int) {
      iterator old = *this;
      node_ = FindNext(node_);
      return old;
    }

    iterator &operator++() {
      node_ = FindNext(node_);
      return *this;
    }

    iterator operator--(int) {
      if (node_ == nullptr) {
        node_ = map_ptr_->max_node_;
        return *this;
      }
      iterator old = *this;
      node_ = FindPrev(node_);
      return old;
    }

    iterator &operator--() {
      if (node_ == nullptr) {
        node_ = map_ptr_->max_node_;;
        return *this;
      }
      node_ = FindPrev(node_);
      return *this;
    }

    value_type &operator*() const {
      return node_->data_;
    }

    bool operator==(const iterator &rhs) const {
      return map_ptr_ == rhs.map_ptr_ && node_ == rhs.node_;
    }

    bool operator==(const const_iterator &rhs) const {
      return map_ptr_ == rhs.map_ptr_ && node_ == rhs.node_;
    }
    
    bool operator!=(const iterator &rhs) const {
      return map_ptr_ != rhs.map_ptr_ || node_ != rhs.node_;
    }

    bool operator!=(const const_iterator &rhs) const {
      return map_ptr_ != rhs.map_ptr_ || node_ != rhs.node_;
    }

    value_type *operator->() const noexcept {
      return &(node_->data_);
    }
  };

  class const_iterator {
   private:
    friend Map<Key, T, KeyCompare>;

    const_iterator(RBTreeNode *node, const Map<Key, T, KeyCompare> *map_ptr) : node_(node), map_ptr_(map_ptr) {}

    const RBTreeNode *node_;
    const Map<Key, T, KeyCompare> *map_ptr_;
    
   public:
    const_iterator() : node_(nullptr) {}

    const_iterator(const const_iterator &other) : node_(other.node_) {}

    const_iterator(const iterator &other) : node_(other.node_) {}

    const_iterator operator++(int) {
      const_iterator old = *this;
      node_ = FindNext(node_);
      return old;
    }

    const_iterator &operator++() {
      node_ = FindNext(node_);
      return *this;
    }

    const_iterator operator--(int) {
      if (node_ == nullptr) {
        node_ = map_ptr_->max_node_;
        return *this;
      }
      const_iterator old = *this;
      node_ = FindPrev(node_);
      return old;
    }

    const_iterator &operator--() {
      if (node_ == nullptr) {
        node_ = map_ptr_->max_node_;
        return *this;
      }
      node_ = FindPrev(node_);
      return *this;
    }

    const value_type &operator*() const {
      return node_->data_;
    }

    bool operator==(const iterator &rhs) const {
      return map_ptr_ == rhs.map_ptr_ && node_ == rhs.node_;
    }

    bool operator==(const const_iterator &rhs) const {
      return map_ptr_ == rhs.map_ptr_ && node_ == rhs.node_;
    }

    bool operator!=(const iterator &rhs) const {
      return map_ptr_ != rhs.map_ptr_ || node_ != rhs.node_;
    }

    bool operator!=(const const_iterator &rhs) const {
      return map_ptr_ != rhs.map_ptr_ || node_ != rhs.node_;
    }

    const value_type *operator->() const noexcept {
      return &(node_->data_);
    }
  };

  Map() : size_(0), root_(nullptr), min_node_(nullptr), max_node_(nullptr) {}

  Map(const Map &other) : size_(other.size_) {
    root_ = CopyTree(other.root_);
    min_node_ = (root_ == nullptr ? nullptr : GetMinimum(root_));
    max_node_ = (root_ == nullptr ? nullptr : GetMaximum(root_));
  }

  Map &operator=(const Map &other) {
    if (this == &other) {
      return *this;
    }
    ClearTree(root_);
    root_ = CopyTree(other.root_);
    min_node_ = (root_ == nullptr ? nullptr : GetMinimum(root_));
    max_node_ = (root_ == nullptr ? nullptr : GetMaximum(root_));
    size_ = other.size_;
    return *this;
  }

  ~Map() {
    ClearTree(root_);
  }

  T &at(const Key &key) {
    RBTreeNode *res = FindNode(key);
    return res->data_.second;
  }

  const T &at(const Key &key) const {
    RBTreeNode *res = FindNode(key);
    return res->data_.second;
  }

  T &operator[](const Key &key) {
    RBTreeNode *res = FindNode(key);
    if (res == nullptr) {
      res = InsertNode(Pair<const Key, T>(key, T())).first;
      size_++;
      if (min_node_ == nullptr || KeyCompare()(res->data_.first, min_node_->data_.first)) {
        min_node_ = res;
      }
    }
    return res->data_.second;
  }

  const T &operator[](const Key &key) const {
    RBTreeNode *res = FindNode(key);
    return res->data_.second;
  }

  iterator begin() {
    return iterator(min_node_, this);
  }

  const_iterator cbegin() const {
    return const_iterator(min_node_, this);
  }

  iterator end() {
    return iterator(nullptr, this);
  }

  const_iterator cend() const {
    return const_iterator(nullptr, this);
  }

  bool empty() const {
    return size_ == 0;
  }

  size_t size() const {
    return size_;
  }

  void clear() {
    ClearTree(root_);
    root_ = nullptr;
    min_node_ = nullptr;
    max_node_ = nullptr;
    size_ = 0;
  }

  Pair<iterator, bool> insert(const value_type &value) {
    Pair<RBTreeNode *, bool> res = InsertNode(value);
    if (res.second) {
      size_++;
      if (min_node_ == nullptr || KeyCompare()(value.first, min_node_->data_.first)) {
        min_node_ = res.first;
      }
      if (max_node_ == nullptr || KeyCompare()(max_node_->data_.first, value.first)) {
        max_node_ = res.first;
      }
    }
    return {iterator(res.first, this), res.second};
  }

  void erase(iterator pos) {
    EraseNode(pos.node_);
    if (pos.node_ == min_node_) {
      min_node_ = (root_ == nullptr ? nullptr : GetMinimum(root_));
    }
    if (pos.node_ == max_node_) {
      max_node_ = (root_ == nullptr ? nullptr : GetMaximum(root_));
    }
    size_--;
  }

  size_t count(const Key &key) const {
    return FindNode(key) == nullptr ? 0 : 1;
  }

  iterator find(const Key &key) {
    return iterator(FindNode(key), this);
  }

  const_iterator find(const Key &key) const {
    return const_iterator(FindNode(key), this);
  }
};

}

#endif //TICKET_SYSTEM_MAP_H
