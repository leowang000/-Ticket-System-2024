#ifndef TICKET_SYSTEM_VECTOR_H
#define TICKET_SYSTEM_VECTOR_H

#include <cstring>
#include <iostream>

namespace bubble {

template<typename T, typename Allocator = std::allocator<T>>
class Vector {
 public:
  class const_iterator;

  class iterator {
   public:
    iterator(T *data, const Vector<T> *vec_ptr) : data_(data), vec_ptr_(vec_ptr) {}

    iterator(const iterator &other) : data_(other.data_), vec_ptr_(other.vec_ptr_) {}

    iterator &operator=(const iterator &other) = default;

    iterator operator+(const int &n) const {
      return {data_ + n, vec_ptr_};
    }

    iterator operator-(const int &n) const {
      return {data_ - n, vec_ptr_};
    }

    int operator-(const iterator &rhs) const {
      return data_ - rhs.data_;
    }

    iterator &operator+=(const int &n) {
      data_ += n;
      return *this;
    }

    iterator &operator-=(const int &n) {
      data_ -= n;
      return *this;
    }

    iterator operator++(int) {
      iterator old = *this;
      ++data_;
      return old;
    }

    iterator &operator++() {
      ++data_;
      return *this;
    }

    iterator operator--(int) {
      iterator old = *this;
      --data_;
      return old;
    }

    iterator &operator--() {
      --data_;
      return *this;
    }

    T &operator*() const {
      return *data_;
    }

    bool operator==(const iterator &rhs) const {
      return data_ == rhs.data_;
    }

    bool operator==(const const_iterator &rhs) const {
      return data_ == rhs.data_;
    }

    bool operator!=(const iterator &rhs) const {
      return data_ != rhs.data_;
    }

    bool operator!=(const const_iterator &rhs) const {
      return data_ != rhs.data_;
    }

   private:
    T *data_;
    const Vector<T> *vec_ptr_;
  };

  class const_iterator {
   public:
    const_iterator(const T *data, const Vector<T> *vec_ptr) : data_(data), vec_ptr_(vec_ptr) {}

    const_iterator(const iterator &other) : data_(other.data_), vec_ptr_(other.vec_ptr_) {}

    const_iterator &operator=(const const_iterator &other) = default;

    const_iterator operator+(const int &n) const {
      return {data_ + n, vec_ptr_};
    }

    const_iterator operator-(const int &n) const {
      return {data_ - n, vec_ptr_};
    }

    int operator-(const const_iterator &rhs) const {
      return data_ - rhs.data_;
    }

    const_iterator &operator+=(const int &n) {
      data_ += n;
      return *this;
    }

    const_iterator &operator-=(const int &n) {
      data_ -= n;
      return *this;
    }

    const_iterator operator++(int) {
      iterator old = *this;
      ++data_;
      return old;
    }

    const_iterator &operator++() {
      ++data_;
      return *this;
    }

    const_iterator operator--(int) {
      iterator old = *this;
      --data_;
      return old;
    }

    const_iterator &operator--() {
      --data_;
      return *this;
    }

    const T &operator*() const {
      return *data_;
    }

    bool operator==(const iterator &rhs) const {
      return data_ == rhs.data_;
    }

    bool operator==(const const_iterator &rhs) const {
      return data_ == rhs.data_;
    }

    bool operator!=(const iterator &rhs) const {
      return data_ != rhs.data_;
    }

    bool operator!=(const const_iterator &rhs) const {
      return data_ != rhs.data_;
    }

   private:
    const T *data_;
    const Vector<T> *vec_ptr_;
  };

  Vector() : alloc_(), first_(alloc_.allocate(kMinCapacity)), size_(0), capacity_(kMinCapacity) {}

  Vector(const Vector &other) : alloc_() {
    first_ = alloc_.allocate(other.capacity_);
    size_ = other.size_;
    capacity_ = other.capacity_;
    Copy(first_, other.first_, other.size_);
  }

  ~Vector() {
    Release();
  }

  Vector &operator=(const Vector &other) {
    if (this == &other) {
      return *this;
    }
    Release();
    first_ = alloc_.allocate(other.capacity_);
    size_ = other.size_;
    capacity_ = other.capacity_;
    Copy(first_, other.first_, other.size_);
    return *this;
  }

  T &at(const size_t &pos) {
    return first_[pos];
  }

  const T &at(const size_t &pos) const {
    return first_[pos];
  }

  T &operator[](const size_t &pos) {
    return first_[pos];
  }

  const T &operator[](const size_t &pos) const {
    return first_[pos];
  }

  T &front() {
    return first_[0];
  }

  const T &front() const {
    return first_[0];
  }

  T &back() {
    return first_[size_ - 1];
  }

  const T &back() const {
    return first_[size_ - 1];
  }

  iterator begin() {
    return {first_, this};
  }

  const_iterator cbegin() const {
    return {first_, this};
  }

  iterator end() {
    return {first_ + size_, this};
  }

  const_iterator cend() const {
    return {first_ + size_, this};
  }

  bool empty() const {
    return size_ == 0;
  }

  size_t size() const {
    return size_;
  }

  void clear() {
    Release();
    first_ = alloc_.allocate(kMinCapacity);
    size_ = 0;
    capacity_ = kMinCapacity;
  }

  iterator insert(iterator pos, const T &value) {
    return insert(pos - begin(), value);
  }

  iterator insert(const size_t &ind, const T &value) {
    if (size_ < capacity_ - 1) {
      for (int i = size_; i > ind; i--) {
        new(first_ + i) T(first_[i - 1]);
      }
      new(first_ + ind) T(value);
      size_++;
    }
    else {
      T *new_space = alloc_.allocate(2 * capacity_);
      Copy(new_space, first_, ind);
      new(new_space + ind) T(value);
      Copy(new_space + ind + 1, first_ + ind, size_ - ind);
      Release();
      first_ = new_space;
      size_++;
      capacity_ *= 2;
    }
    return {first_ + ind, this};
  }

  iterator erase(iterator pos) {
    return erase(pos - begin());
  }

  iterator erase(const size_t &ind) {
    if (capacity_ > kMinCapacity && size_ + 1 <= capacity_ / 4) {
      size_t new_capacity = std::max(capacity_ / 4, kMinCapacity);
      T *new_space = alloc_.allocate(new_capacity);
      Copy(new_space, first_, ind);
      Copy(new_space + ind, first_ + ind + 1, size_ - 1 - ind);
      Release();
      first_ = new_space;
      size_--;
      capacity_ = new_capacity;
    }
    else {
      for (int i = ind; i <= size_ - 2; i++) {
        new(first_ + i) T(first_[i + 1]);
      }
      (first_ + size_ - 1)->~T();
      size_--;
    }
    return {first_ + ind, this};
  }

  void push_back(const T &value) {
    insert(size_, value);
  }

  void pop_back() {
    erase(size_ - 1);
  }

  T *data() {
    return first_;
  }

  const T *data() const {
    return first_;
  }

 private:
  static const size_t kMinCapacity = 8;

  Allocator alloc_;
  T *first_;
  size_t size_, capacity_;

  void Release() {
    for (int i = 0; i < size_; i++) {
      (first_ + i)->~T();
    }
    alloc_.deallocate(first_, capacity_);
  }

  void Copy(T *dest, T *src, size_t n) {
    for (int i = 0; i < n; i++) {
      new(dest + i) T(src[i]);
    }
  }
};

}

#endif //TICKET_SYSTEM_VECTOR_H