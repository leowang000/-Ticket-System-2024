#ifndef TICKET_SYSTEM_2024_BPT_H
#define TICKET_SYSTEM_2024_BPT_H

#include <string>

#include "../utility/File/File.h"
#include "../utility/STLite/Vector.h"
#include "../utility/STLite/Pair.h"
#include "../utility/STLite/Algorithm.h"
#include "../utility/STLite/String.h"

namespace bubble {

template<class Key, class T>
class BPlusTree {
 public:
  using ValueType = Pair<Key, T>;

  explicit BPlusTree(const std::string &file_name);
  ~BPlusTree();

  Vector<T> Find(const Key &key);
  void Insert(const ValueType &value);
  void Assign(const ValueType &old, const T &value);
  void Erase(const ValueType &value);
  void Clear();

 private:
  static constexpr int PageSize = 4096;
  static constexpr int MaxNodeSize =
      (PageSize + sizeof(Key) - sizeof(int) - sizeof(bool)) / (sizeof(int) + sizeof(ValueType));
  static constexpr int MinNodeSize = (MaxNodeSize + 1) / 2;
  static constexpr int MaxLeafSize = (PageSize - 2 * sizeof(int)) / sizeof(ValueType);
  static constexpr int MinLeafSize = (MaxLeafSize + 1) / 2;

  struct Node {
    int size_;
    bool is_leaf_;
    int children_[MaxNodeSize];
    ValueType data_[MaxNodeSize - 1];

    explicit Node(int size = 0, bool is_leaf = false);
  };

  struct Leaf {
    int size_;
    int next_;
    ValueType data_[MaxLeafSize];

    explicit Leaf(int size = 0, int next = 0);
  };

  bool
  InsertAdjustLeaf(Node &node, int node_ptr, int leaf_id, int now_leaf_ptr, const ValueType &value, int &new_leaf_ptr,
                   ValueType &new_leaf_min);
  bool InsertAdjustNode(Node &node, int now_node_ptr, int id, int &new_node_ptr, ValueType &new_node_min);
  int DeleteAdjustLeaf(Vector<Node> &nodes, const Vector<int> &ids, const ValueType &value);
  bool DeleteAdjustNode(Node &node, int &id, Node &parent, int parent_ptr, int ptr_id);
  void DeleteAdjustRoot(Node &node, int id);

  int root_, head_;
  Storage<Node, 1> node_file_;
  Storage<Leaf, 1> leaf_file_;
};

template<class Key, class T>
BPlusTree<Key, T>::BPlusTree(const std::string &file_name) :
    node_file_(file_name + "_node"), leaf_file_(file_name + "_leaf") {
  if (node_file_.IsCreated()) {
    node_file_.ReadInt(root_, 1);
    leaf_file_.ReadInt(head_, 1);
  }
  else {
    root_ = 0;
    head_ = 0;
  }
}

template<class Key, class T>
BPlusTree<Key, T>::~BPlusTree() {
  node_file_.WriteInt(root_, 1);
  leaf_file_.WriteInt(head_, 1);
}

template<class Key, class T>
Vector<T> BPlusTree<Key, T>::Find(const Key &key) {
  Vector<T> result;
  if (root_ == 0) {
    return result;
  }
  int now = root_;
  while (true) {
    static Node node;
    node_file_.Read(node, now);
    int id =
        LowerBound(node.data_, node.data_ + node.size_ - 1, ValueType(key, T()), typename ValueType::CompareByKey()) -
        node.data_;
    now = node.children_[id];
    if (node.is_leaf_) {
      break;
    }
  }
  while (true) {
    static Leaf leaf;
    leaf_file_.Read(leaf, now);
    ValueType *first = LowerBound(leaf.data_, leaf.data_ + leaf.size_, ValueType(key, T()),
                                  typename ValueType::CompareByKey());
    ValueType *last = UpperBound(leaf.data_, leaf.data_ + leaf.size_, ValueType(key, T()),
                                 typename ValueType::CompareByKey());
    for (ValueType *it = first; it != last; it++) {
      result.push_back(it->second);
    }
    now = leaf.next_;
    if (last != leaf.data_ + leaf.size_ || now == 0) {
      break;
    }
  }
  return result;
}

template<class Key, class T>
void BPlusTree<Key, T>::Insert(const ValueType &value) {
  if (root_ == 0) {
    Leaf leaf(1);
    leaf.data_[0] = value;
    head_ = leaf_file_.Write(leaf);
    Node node(1, true);
    node.children_[0] = head_;
    root_ = node_file_.Write(node);
    return;
  }
  int now = root_;
  Vector<Node> nodes;
  Vector<int> node_ptr, ids;
  while (true) {
    static Node node;
    node_file_.Read(node, now);
    node_ptr.push_back(now);
    int id = UpperBound(node.data_, node.data_ + node.size_ - 1, value) - node.data_;
    if (id >= 1 && value == node.data_[id - 1]) {
      return;
    }
    ids.push_back(id);
    nodes.push_back(node);
    now = node.children_[id];
    if (node.is_leaf_) {
      break;
    }
  }
  int new_node_ptr;
  ValueType new_node_min;
  if (InsertAdjustLeaf(nodes.back(), node_ptr.back(), ids.back(), now, value, new_node_ptr, new_node_min)) {
    for (int i = nodes.size() - 1; i >= 0; i--) {
      if (!InsertAdjustNode(nodes[i], node_ptr[i], ids[i], new_node_ptr, new_node_min)) {
        break;
      }
      if (i == 0) {
        Node new_root_node(2);
        new_root_node.children_[0] = root_;
        new_root_node.children_[1] = new_node_ptr;
        new_root_node.data_[0] = new_node_min;
        root_ = node_file_.Write(new_root_node);
      }
    }
  }
}

template<class Key, class T>
void BPlusTree<Key, T>::Assign(const ValueType &old, const T &value) {
  int now = root_;
  while (true) {
    static Node node;
    node_file_.Read(node, now);
    int id = UpperBound(node.data_, node.data_ + node.size_ - 1, old) - node.data_;
    now = node.children_[id];
    if (node.is_leaf_) {
      break;
    }
  }
  static Leaf leaf;
  leaf_file_.Read(leaf, now);
  LowerBound(leaf.data_, leaf.data_ + leaf.size_, old)->second = value;
  leaf_file_.Write(leaf, now);
}

template<class Key, class T>
void BPlusTree<Key, T>::Erase(const ValueType &value) {
  if (root_ == 0) {
    return;
  }
  int now = root_;
  Vector<Node> nodes;
  Vector<int> node_ptr, ids;
  while (true) {
    static Node node;
    node_file_.Read(node, now);
    node_ptr.push_back(now);
    int id = UpperBound(node.data_, node.data_ + node.size_ - 1, value) - node.data_;
    ids.push_back(id);
    nodes.push_back(node);
    now = node.children_[id];
    if (node.is_leaf_) {
      break;
    }
  }
  int id = DeleteAdjustLeaf(nodes, ids, value);
  if (id != -1) {
    for (int i = nodes.size() - 1; i >= 0; i--) {
      if (i > 0 && !DeleteAdjustNode(nodes[i], id, nodes[i - 1], node_ptr[i - 1], ids[i - 1])) {
        break;
      }
      if (i == 0) {
        DeleteAdjustRoot(nodes[i], id);
      }
    }
  }
}

template<class Key, class T>
void BPlusTree<Key, T>::Clear() {
  node_file_.Clear();
  leaf_file_.Clear();
  root_ = 0;
  head_ = 0;
}

template<class Key, class T>
BPlusTree<Key, T>::Node::Node(int size, bool is_leaf) : size_(size), is_leaf_(is_leaf), children_(), data_() {}

template<class Key, class T>
BPlusTree<Key, T>::Leaf::Leaf(int size, int next) : size_(size), next_(next), data_() {}

template<class Key, class T>
bool
BPlusTree<Key, T>::InsertAdjustLeaf(Node &node, int node_ptr, int leaf_id, int now_leaf_ptr, const ValueType &value,
                                    int &new_leaf_ptr, ValueType &new_leaf_min) {
  static Leaf leaf;
  leaf_file_.Read(leaf, now_leaf_ptr);
  int id = LowerBound(leaf.data_, leaf.data_ + leaf.size_, value) - leaf.data_;
  if (id != leaf.size_ && leaf.data_[id] == value) {
    return false;
  }
  if (leaf.size_ < MaxLeafSize) {
    memmove(leaf.data_ + id + 1, leaf.data_ + id, (leaf.size_ - id) * sizeof(ValueType));
    leaf.data_[id] = value;
    leaf.size_++;
    leaf_file_.Write(leaf, now_leaf_ptr);
    if (id == 0 && leaf_id > 0) {
      node.data_[leaf_id - 1] = value;
      node_file_.Write(node, node_ptr);
    }
    return false;
  }
  leaf.size_ = MinLeafSize;
  Leaf new_leaf(MaxLeafSize + 1 - leaf.size_, leaf.next_);
  if (id < leaf.size_) {
    memcpy(new_leaf.data_, leaf.data_ + leaf.size_ - 1, new_leaf.size_ * sizeof(ValueType));
    memmove(leaf.data_ + id + 1, leaf.data_ + id, (leaf.size_ - id - 1) * sizeof(ValueType));
    leaf.data_[id] = value;
  }
  else {
    memcpy(new_leaf.data_, leaf.data_ + leaf.size_, (id - leaf.size_) * sizeof(ValueType));
    new_leaf.data_[id - leaf.size_] = value;
    memcpy(new_leaf.data_ + id - leaf.size_ + 1, leaf.data_ + id, (MaxLeafSize - id) * sizeof(ValueType));
  }
  new_leaf_ptr = leaf_file_.Write(new_leaf);
  new_leaf_min = new_leaf.data_[0];
  leaf.next_ = new_leaf_ptr;
  leaf_file_.Write(leaf, now_leaf_ptr);
  return true;
}

template<class Key, class T>
bool
BPlusTree<Key, T>::InsertAdjustNode(Node &node, int now_node_ptr, int id, int &new_node_ptr, ValueType &new_node_min) {
  if (node.size_ < MaxNodeSize) {
    memmove(node.children_ + id + 2, node.children_ + id + 1, (node.size_ - id - 1) * sizeof(int));
    node.children_[id + 1] = new_node_ptr;
    memmove(node.data_ + id + 1, node.data_ + id, (node.size_ - id - 1) * sizeof(ValueType));
    node.data_[id] = new_node_min;
    node.size_++;
    node_file_.Write(node, now_node_ptr);
    return false;
  }
  node.size_ = MinNodeSize;
  Node new_node(MaxNodeSize + 1 - node.size_, node.is_leaf_);
  if (id <= node.size_ - 2) {
    memcpy(new_node.children_, node.children_ + node.size_ - 1, new_node.size_ * sizeof(int));
    memmove(node.children_ + id + 2, node.children_ + id + 1, (node.size_ - id - 2) * sizeof(int));
    node.children_[id + 1] = new_node_ptr;
  }
  else {
    memcpy(new_node.children_, node.children_ + node.size_, (id - node.size_ + 1) * sizeof(int));
    new_node.children_[id - node.size_ + 1] = new_node_ptr;
    memcpy(new_node.children_ + id - node.size_ + 2, node.children_ + id + 1, (MaxNodeSize - id - 1) * sizeof(int));
  }
  if (id == node.size_ - 1) {
    memcpy(new_node.data_, node.data_ + id, (new_node.size_ - 1) * sizeof(ValueType));
  }
  else if (id < node.size_ - 1) {
    memcpy(new_node.data_, node.data_ + node.size_ - 1, (new_node.size_ - 1) * sizeof(ValueType));
    ValueType tmp = node.data_[node.size_ - 2];
    memmove(node.data_ + id + 1, node.data_ + id, (node.size_ - id - 2) * sizeof(ValueType));
    node.data_[id] = new_node_min;
    new_node_min = tmp;
  }
  else {
    memcpy(new_node.data_, node.data_ + node.size_, (id - node.size_) * sizeof(ValueType));
    new_node.data_[id - node.size_] = new_node_min;
    memcpy(new_node.data_ + id - node.size_ + 1, node.data_ + id, (MaxNodeSize - id - 1) * sizeof(ValueType));
    new_node_min = node.data_[node.size_ - 1];
  }
  new_node_ptr = node_file_.Write(new_node);
  node_file_.Write(node, now_node_ptr);
  return true;
}

template<class Key, class T>
int BPlusTree<Key, T>::DeleteAdjustLeaf(Vector<Node> &nodes, const Vector<int> &ids, const ValueType &value) {
  Node &parent = nodes.back();
  int ptr_id = ids.back(), now_leaf_ptr = parent.children_[ptr_id];
  static Leaf leaf;
  leaf_file_.Read(leaf, now_leaf_ptr);
  int id = LowerBound(leaf.data_, leaf.data_ + leaf.size_, value) - leaf.data_;
  if (id == leaf.size_ || leaf.data_[id] != value) {
    return -1;
  }
  if (id == 0) {
    for (int i = nodes.size() - 1; i >= 0; i--) {
      if (ids[i] != 0) {
        nodes[i].data_[ids[i] - 1] = leaf.data_[1];
        node_file_.Write(nodes[i], (i == 0 ? root_ : nodes[i - 1].children_[ids[i - 1]]));
        break;
      }
    }
  }
  if (parent.size_ == 1 || leaf.size_ > MinLeafSize) {
    memmove(leaf.data_ + id, leaf.data_ + id + 1, (leaf.size_ - id - 1) * sizeof(ValueType));
    leaf.size_--;
    if (leaf.size_ > 0) {
      leaf_file_.Write(leaf, now_leaf_ptr);
    }
    else {
      node_file_.Erase(root_);
      leaf_file_.Erase(head_);
      root_ = 0;
      head_ = 0;
    }
    return -1;
  }
  static Leaf left_leaf, right_leaf;
  int left_leaf_ptr, right_leaf_ptr;
  bool have_left_sibling = (ptr_id != 0), have_right_sibling = (ptr_id != parent.size_ - 1);
  if (have_left_sibling) {
    left_leaf_ptr = parent.children_[ptr_id - 1];
    leaf_file_.Read(left_leaf, left_leaf_ptr);
    if (left_leaf.size_ > MinLeafSize) {
      int sum = left_leaf.size_ + leaf.size_ - 1, new_left_leaf_size = sum / 2, new_size = sum - sum / 2;
      memmove(leaf.data_ + id + new_size - leaf.size_ + 1, leaf.data_ + id + 1,
              (leaf.size_ - id - 1) * sizeof(ValueType));
      memmove(leaf.data_ + left_leaf.size_ - new_left_leaf_size, leaf.data_, id * sizeof(ValueType));
      memcpy(leaf.data_, left_leaf.data_ + new_left_leaf_size,
             (left_leaf.size_ - new_left_leaf_size) * sizeof(ValueType));
      parent.data_[ptr_id - 1] = leaf.data_[0];
      leaf.size_ = new_size;
      left_leaf.size_ = new_left_leaf_size;
      leaf_file_.Write(leaf, now_leaf_ptr);
      leaf_file_.Write(left_leaf, left_leaf_ptr);
      node_file_.Write(parent, (nodes.size() == 1 ? root_ : nodes[nodes.size() - 2].children_[ids[ids.size() - 2]]));
      return -1;
    }
  }
  if (have_right_sibling) {
    right_leaf_ptr = parent.children_[ptr_id + 1];
    leaf_file_.Read(right_leaf, right_leaf_ptr);
    if (right_leaf.size_ > MinLeafSize) {
      int sum = right_leaf.size_ + leaf.size_ - 1, new_size = sum / 2, new_right_leaf_size = sum - sum / 2;
      memmove(leaf.data_ + id, leaf.data_ + id + 1, (leaf.size_ - id - 1) * sizeof(ValueType));
      memcpy(leaf.data_ + leaf.size_ - 1, right_leaf.data_,
             (right_leaf.size_ - new_right_leaf_size) * sizeof(ValueType));
      memmove(right_leaf.data_, right_leaf.data_ + right_leaf.size_ - new_right_leaf_size,
              new_right_leaf_size * sizeof(ValueType));
      parent.data_[ptr_id] = right_leaf.data_[0];
      leaf.size_ = new_size;
      right_leaf.size_ = new_right_leaf_size;
      leaf_file_.Write(leaf, now_leaf_ptr);
      leaf_file_.Write(right_leaf, right_leaf_ptr);
      node_file_.Write(parent, (nodes.size() == 1 ? root_ : nodes[nodes.size() - 2].children_[ids[ids.size() - 2]]));
      return -1;
    }
  }
  if (have_left_sibling) {
    memcpy(left_leaf.data_ + MinLeafSize, leaf.data_, id * sizeof(ValueType));
    memcpy(left_leaf.data_ + MinLeafSize + id, leaf.data_ + id + 1, (MinLeafSize - id - 1) * sizeof(ValueType));
    left_leaf.size_ = MinLeafSize * 2 - 1;
    parent.children_[ptr_id] = left_leaf_ptr;
    left_leaf.next_ = leaf.next_;
    leaf_file_.Write(left_leaf, left_leaf_ptr);
    leaf_file_.Erase(now_leaf_ptr);
    return ptr_id - 1;
  }
  if (have_right_sibling) {
    memmove(leaf.data_ + id, leaf.data_ + id + 1, (MinLeafSize - id - 1) * sizeof(ValueType));
    memcpy(leaf.data_ + MinLeafSize - 1, right_leaf.data_, MinLeafSize * sizeof(ValueType));
    leaf.size_ = MinLeafSize * 2 - 1;
    parent.children_[ptr_id + 1] = now_leaf_ptr;
    leaf.next_ = right_leaf.next_;
    leaf_file_.Write(leaf, now_leaf_ptr);
    leaf_file_.Erase(right_leaf_ptr);
    return ptr_id;
  }
}

template<class Key, class T>
bool BPlusTree<Key, T>::DeleteAdjustNode(Node &node, int &id, Node &parent, int parent_ptr, int ptr_id) {
  int now_node_ptr = parent.children_[ptr_id];
  if (node.size_ > MinNodeSize) {
    memmove(node.children_ + id, node.children_ + id + 1, (node.size_ - id - 1) * sizeof(int));
    memmove(node.data_ + id, node.data_ + id + 1, (node.size_ - id - 2) * sizeof(ValueType));
    node.size_--;
    node_file_.Write(node, now_node_ptr);
    return false;
  }
  static Node left_node, right_node;
  int left_node_ptr, right_node_ptr;
  bool have_left_sibling = (ptr_id != 0), have_right_sibling = (ptr_id != parent.size_ - 1);
  if (have_left_sibling) {
    left_node_ptr = parent.children_[ptr_id - 1];
    node_file_.Read(left_node, left_node_ptr);
    if (left_node.size_ > MinNodeSize) {
      int sum = left_node.size_ + node.size_ - 1, new_left_node_size = sum / 2, new_size = sum - sum / 2;
      memmove(node.children_ + new_size - node.size_ + id + 1, node.children_ + id + 1,
              (node.size_ - id - 1) * sizeof(int));
      memmove(node.children_ + new_size - node.size_ + 1, node.children_, id * sizeof(int));
      memcpy(node.children_, left_node.children_ + new_left_node_size,
             (left_node.size_ - new_left_node_size) * sizeof(int));
      memmove(node.data_ + new_size - node.size_ + id + 1, node.data_ + id + 1,
              (node.size_ - id - 2) * sizeof(ValueType));
      memmove(node.data_ + new_size - node.size_ + 1, node.data_, id * sizeof(ValueType));
      node.data_[new_size - node.size_] = parent.data_[ptr_id - 1];
      memcpy(node.data_, left_node.data_ + new_left_node_size,
             (left_node.size_ - new_left_node_size - 1) * sizeof(ValueType));
      parent.data_[ptr_id - 1] = left_node.data_[new_left_node_size - 1];
      node.size_ = new_size;
      left_node.size_ = new_left_node_size;
      node_file_.Write(node, now_node_ptr);
      node_file_.Write(left_node, left_node_ptr);
      node_file_.Write(parent, parent_ptr);
      return false;
    }
  }
  if (have_right_sibling) {
    right_node_ptr = parent.children_[ptr_id + 1];
    node_file_.Read(right_node, right_node_ptr);
    if (right_node.size_ > MinNodeSize) {
      int sum = right_node.size_ + node.size_ - 1, new_right_node_size = sum / 2, new_size = sum - sum / 2;
      memmove(node.children_ + id, node.children_ + id + 1, (node.size_ - id - 1) * sizeof(int));
      memcpy(node.children_ + node.size_ - 1, right_node.children_,
             (right_node.size_ - new_right_node_size) * sizeof(int));
      memmove(right_node.children_, right_node.children_ + right_node.size_ - new_right_node_size,
              new_right_node_size * sizeof(int));
      memmove(node.data_ + id, node.data_ + id + 1, (node.size_ - id - 2) * sizeof(ValueType));
      node.data_[node.size_ - 2] = parent.data_[ptr_id];
      memcpy(node.data_ + node.size_ - 1, right_node.data_, (new_size - node.size_) * sizeof(ValueType));
      parent.data_[ptr_id] = right_node.data_[new_size - node.size_];
      memmove(right_node.data_, right_node.data_ + right_node.size_ - new_right_node_size,
              new_right_node_size * sizeof(ValueType));
      node.size_ = new_size;
      right_node.size_ = new_right_node_size;
      node_file_.Write(node, now_node_ptr);
      node_file_.Write(right_node, right_node_ptr);
      node_file_.Write(parent, parent_ptr);
      return false;
    }
  }
  if (have_left_sibling) {
    memcpy(left_node.children_ + MinNodeSize, node.children_, id * sizeof(int));
    memcpy(left_node.children_ + MinNodeSize + id, node.children_ + id + 1, (MinNodeSize - id - 1) * sizeof(int));
    left_node.data_[MinNodeSize - 1] = parent.data_[ptr_id - 1];
    memcpy(left_node.data_ + MinNodeSize, node.data_, id * sizeof(ValueType));
    memcpy(left_node.data_ + MinNodeSize + id, node.data_ + id + 1, (MinNodeSize - id - 2) * sizeof(ValueType));
    left_node.size_ = MinNodeSize * 2 - 1;
    parent.children_[ptr_id] = left_node_ptr;
    id = ptr_id - 1;
    node_file_.Write(left_node, left_node_ptr);
    node_file_.Erase(now_node_ptr);
    if (root_ == parent_ptr && parent.size_ == 2) {
      node_file_.Erase(root_);
      root_ = left_node_ptr;
      return false;
    }
    return true;
  }
  if (have_right_sibling) {
    memmove(node.children_ + id, node.children_ + id + 1, (MinNodeSize - id - 1) * sizeof(int));
    memcpy(node.children_ + MinNodeSize - 1, right_node.children_, MinNodeSize * sizeof(int));
    memmove(node.data_ + id, node.data_ + id + 1, (MinNodeSize - id - 2) * sizeof(ValueType));
    node.data_[MinNodeSize - 2] = parent.data_[ptr_id];
    memcpy(node.data_ + MinNodeSize - 1, right_node.data_, (MinNodeSize - 1) * sizeof(ValueType));
    node.size_ = MinNodeSize * 2 - 1;
    parent.children_[ptr_id + 1] = now_node_ptr;
    id = ptr_id;
    node_file_.Write(node, now_node_ptr);
    node_file_.Erase(right_node_ptr);
    if (root_ == parent_ptr && parent.size_ == 2) {
      node_file_.Erase(root_);
      root_ = now_node_ptr;
      return false;
    }
    return true;
  }
}

template<class Key, class T>
void BPlusTree<Key, T>::DeleteAdjustRoot(Node &node, int id) {
  memmove(node.children_ + id, node.children_ + id + 1, (node.size_ - id - 1) * sizeof(int));
  memmove(node.data_ + id, node.data_ + id + 1, (node.size_ - id - 2) * sizeof(ValueType));
  node.size_--;
  node_file_.Write(node, root_);
}

}

#endif //TICKET_SYSTEM_2024_BPT_H
