#ifndef TICKET_SYSTEM_ARRAY_H
#define TICKET_SYSTEM_ARRAY_H

#include <iostream>
#include <sstream>

namespace bubble {

template<class T, std::size_t n> // n must be greater than zero
class Array {
 public:
  Array() = default;
  Array(const Array &other) = default;
  ~Array() = default;
  explicit Array(const std::string &str, char ch = '|', const std::string &null_string = "_");

  T &operator[](std::size_t pos);
  const T& operator[](std::size_t pos) const;
  T *data();
  const T *data() const;
  std::size_t size() const;

 private:
  T data_[n];
};

template<class T, std::size_t n>
Array<T, n>::Array(const std::string &str, char ch, const std::string &null_string) {
  if (str != null_string) {
    int len = str.size(), ptr = 0;
    std::string substring;
    for (int i = 0; i < len; i++) {
      if (str[i] != ch) {
        substring += str[i];
      }
      if (str[i] == ch || i == len - 1) {
        std::stringstream sstr(substring);
        sstr >> data_[ptr++];
        substring.clear();
      }
    }
  }
}

template<class T, std::size_t n>
T &Array<T, n>::operator[](std::size_t pos) {
  return data_[pos];
}

template<class T, std::size_t n>
const T &Array<T, n>::operator[](std::size_t pos) const {
  return data_[pos];
}

template<class T, std::size_t n>
T *Array<T, n>::data() {
  return data_;
}

template<class T, std::size_t n>
const T *Array<T, n>::data() const {
  return data_;
}

template<class T, std::size_t n>
std::size_t Array<T, n>::size() const {
  return n;
}

}

#endif //TICKET_SYSTEM_ARRAY_H
