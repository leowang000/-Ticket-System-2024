#ifndef TICKET_SYSTEM_LIST_H
#define TICKET_SYSTEM_LIST_H

#include <iostream>

namespace bubble {

template<typename T>
class List {
 private:
  struct BaseNode {
    BaseNode *prev_, *next_;

    explicit BaseNode(BaseNode *prev = nullptr, BaseNode *next = nullptr) : prev_(prev), next_(next) {}
  };

  struct Node : public BaseNode {
    T data_;

    explicit Node(const T &data, BaseNode *prev = nullptr, BaseNode *next = nullptr) :
        BaseNode(prev, next), data_(data) {}
  };

  Node *insert(BaseNode *pos, Node *cur) {
    size_++;
    pos->prev_->next_ = cur;
    cur->prev_ = pos->prev_;
    cur->next_ = pos;
    pos->prev_ = cur;
    return cur;
  }

  Node *erase(BaseNode *pos) {
    size_--;
    pos->prev_->next_ = pos->next_;
    pos->next_->prev_ = pos->prev_;
    return static_cast<Node *>(pos);
  }

  BaseNode *head_, *tail_;
  size_t size_;

 public:
  class const_iterator;

  class iterator {
   private:
    friend List<T>;

    explicit iterator(BaseNode *cur) : cur_(cur) {}

    BaseNode *cur_;

   public:
    iterator(const iterator &other) : cur_(other.cur_) {}

    iterator operator++(int) {
      iterator old = *this;
      cur_ = cur_->next_;
      return old;
    }

    iterator &operator++() {
      cur_ = cur_->next_;
      return *this;
    }

    iterator operator--(int) {
      iterator old = *this;
      cur_ = cur_->prev_;
      return old;
    }

    iterator &operator--() {
      cur_ = cur_->prev_;
      return *this;
    }

    T &operator*() const {
      return static_cast<Node *>(cur_)->data_;
    }

    T *operator->() const {
        return &(static_cast<Node *>(cur_)->data_);
    }

    bool operator==(const iterator &rhs) const {
      return cur_ == rhs.cur_;
    }

    bool operator==(const const_iterator &rhs) const {
      return cur_ == rhs.cur_;
    }

    bool operator!=(const iterator &rhs) const {
      return cur_ != rhs.cur_;
    }

    bool operator!=(const const_iterator &rhs) const {
      return cur_ != rhs.cur_;
    }
  };

  class const_iterator {
   private:
    friend List<T>;

    explicit const_iterator(BaseNode *cur) : cur_(cur) {}

    BaseNode *cur_;

   public:
    const_iterator(const const_iterator &other) : cur_(other.cur_) {}

    explicit const_iterator(const iterator &other) : cur_(other.cur_) {}

    const_iterator operator++(int) {
      const_iterator old = *this;
      cur_ = cur_->next_;
      return old;
    }

    const_iterator &operator++() {
      cur_ = cur_->next_;
      return *this;
    }

    const_iterator operator--(int) {
      const_iterator old = *this;
      cur_ = cur_->prev_;
      return old;
    }

    const_iterator &operator--() {
      cur_ = cur_->prev_;
      return *this;
    }

    const T &operator*() const {
      return static_cast<Node *>(cur_)->data_;
    }

    const T *operator->() const {
      return &(static_cast<Node *>(cur_)->data_);
    }

    bool operator==(const iterator &rhs) const {
      return cur_ == rhs.cur_;
    }

    bool operator==(const const_iterator &rhs) const {
      return cur_ == rhs.cur_;
    }

    bool operator!=(const iterator &rhs) const {
      return cur_ != rhs.cur_;
    }

    bool operator!=(const const_iterator &rhs) const {
      return cur_ != rhs.cur_;
    }
  };

  List() : size_(0) {
    head_ = new BaseNode();
    tail_ = new BaseNode();
    head_->next_ = tail_;
    tail_->prev_ = head_;
  }

  List(const List &other) : size_(other.size_) {
    head_ = new BaseNode();
    tail_ = new BaseNode();
    BaseNode *prev_node_ptr = head_;
    for (auto iter = other.cbegin(); iter != other.cend(); ++iter) {
      prev_node_ptr = new Node(*iter, prev_node_ptr);
    }
    tail_->prev_ = prev_node_ptr;
    BaseNode *next_node_ptr = tail_;
    while (prev_node_ptr != nullptr) {
      prev_node_ptr->next_ = next_node_ptr;
      prev_node_ptr = prev_node_ptr->prev_;
      next_node_ptr = next_node_ptr->prev_;
    }
  }

  ~List() {
    List::clear();
    delete head_;
    delete tail_;
  }

  List &operator=(const List &other) {
    if (this == &other) {
      return *this;
    }
    List::clear();
    size_ = other.size_;
    BaseNode *prev_node_ptr = head_;
    for (auto iter = other.cbegin(); iter != other.cend(); ++iter) {
      prev_node_ptr = new Node(*iter, prev_node_ptr);
    }
    tail_->prev_ = prev_node_ptr;
    BaseNode *next_node_ptr = tail_;
    while (prev_node_ptr != nullptr) {
      prev_node_ptr->next_ = next_node_ptr;
      prev_node_ptr = prev_node_ptr->prev_;
      next_node_ptr = next_node_ptr->prev_;
    }
    return *this;
  }

  const T &front() const {
    return static_cast<Node *>(head_->next_)->data_;
  }

  const T &back() const {
    return static_cast<Node *>(tail_->prev_)->data_;
  }

  iterator begin() {
    return iterator(head_->next_);
  }

  const_iterator cbegin() const {
    return const_iterator(head_->next_);
  }

  iterator end() {
    return iterator(tail_);
  }

  const_iterator cend() const {
    return const_iterator(tail_);
  }

  bool empty() const {
    return size_ == 0;
  }

  size_t size() const {
    return size_;
  }

  void clear() {
    BaseNode *node_ptr = head_->next_, *next_node_ptr;
    while (node_ptr != tail_) {
      next_node_ptr = node_ptr->next_;
      delete static_cast<Node *>(node_ptr);
      node_ptr = next_node_ptr;
    }
    size_ = 0;
    head_->next_ = tail_;
    tail_->prev_ = head_;
  }

  iterator insert(iterator pos, const T &value) {
    BaseNode *cur = new Node(value, pos.cur_->prev_, pos.cur_);
    size_++;
    pos.cur_->prev_->next_ = cur;
    pos.cur_->prev_ = cur;
    return iterator(cur);
  }

  iterator erase(iterator pos) {
    size_--;
    pos.cur_->prev_->next_ = pos.cur_->next_;
    pos.cur_->next_->prev_ = pos.cur_->prev_;
    BaseNode *next_iter = pos.cur_->next_;
    delete static_cast<Node *>(pos.cur_);
    return iterator(next_iter);
  }

  void replace(iterator pos, iterator target) { // combination of insert and erase, pos != target
    pos.cur_->prev_->next_ = pos.cur_->next_;
    pos.cur_->next_->prev_ = pos.cur_->prev_;
    pos.cur_->prev_ = target.cur_->prev_;
    pos.cur_->next_ = target.cur_;
    target.cur_->prev_->next_ = pos.cur_;
    target.cur_->prev_ = pos.cur_;
  }

  void push_back(const T &value) {
    insert(tail_, new Node(value));
  }

  void pop_back() {
    delete erase(tail_->prev_);
  }

  void push_front(const T &value) {
    insert(head_->next_, new Node(value));
  }

  void pop_front() {
    delete erase(head_->next_);
  }
};

}

#endif //TICKET_SYSTEM_LIST_H
