#ifndef TICKET_SYSTEM_STRING_H
#define TICKET_SYSTEM_STRING_H

#include <iostream>
#include <cstring>

#include "Pair.h"
#include "Array.h"

namespace bubble {

template<class T>
std::string ToString(T n, int width, char fill = '0', bool fill_left = true) {
  std::string res(std::to_string(n));
  std::string fill_string(width - res.size(), fill);
  return fill_left ? fill_string + res : res + fill_string;
}

template<int len> class String;

template<int len> std::ostream &operator<<(std::ostream &, const String<len> &);
template<int len> std::istream &operator>>(std::istream &, String<len> &);

template<int len>
class String {
 public:
  using HashType = Pair<int, int>;

  String();
  String(const String<len> &);
  explicit String(const char *);
  explicit String(const std::string &);
  String &operator=(const String<len> &);

  char &operator[](int);
  bool operator<(const String<len> &) const;
  bool operator>(const String<len> &) const;
  bool operator<=(const String<len> &) const;
  bool operator==(const String<len> &) const;
  bool operator>=(const String<len> &) const;
  bool operator!=(const String<len> &) const;
  HashType GetHash() const;
  std::string ToString() const;
  friend std::ostream &operator<<<len>(std::ostream &, const String<len> &);
  friend std::istream &operator>><len>(std::istream &, String<len> &);

 private:
  static constexpr unsigned long long b1 = 257;
  static constexpr unsigned long long m1 = 1e9 + 7;
  static constexpr unsigned long long b2 = 263;
  static constexpr unsigned long long m2 = 998244343;

  int Hash(int b, int m) const;

  char str_[len + 1];
};

template<int len>
String<len>::String() : str_() {}

template<int len>
String<len>::String(const String<len> &other) : str_() {
  strncpy(str_, other.str_, len + 1);
}

template<int len>
String<len>::String(const char *other) : str_() {
  int i;
  strcpy(str_, other);
}

template<int len>
String<len>::String(const std::string &other) : String(other.c_str()) {}

template<int len>
String<len> &String<len>::operator=(const String<len> &other) {
  if (this == &other) {
    return *this;
  }
  strncpy(str_, other.str_, len + 1);
  int i;
  for (i = strlen(other.str_); i <= len; i++) {
    str_[i] = '\0';
  }
  return *this;
}

template<int len>
char &String<len>::operator[](int index) {
  return str_[index];
}

template<int len>
bool String<len>::operator<(const String<len> &rhs) const {
  return strncmp(str_, rhs.str_, len + 1) < 0;
}

template<int len>
bool String<len>::operator>(const String<len> &rhs) const {
  return strncmp(str_, rhs.str_, len + 1) > 0;
}

template<int len>
bool String<len>::operator<=(const String<len> &rhs) const {
  return strncmp(str_, rhs.str_, len + 1) <= 0;
}

template<int len>
bool String<len>::operator==(const String<len> &rhs) const {
  return strncmp(str_, rhs.str_, len + 1) == 0;
}

template<int len>
bool String<len>::operator>=(const String<len> &rhs) const {
  return strncmp(str_, rhs.str_, len + 1) >= 0;
}

template<int len>
bool String<len>::operator!=(const String<len> &rhs) const {
  return strncmp(str_, rhs.str_, len + 1) != 0;
}

template<int len>
String<len>::HashType String<len>::GetHash() const {
  return Pair<int, int>(Hash(b1, m1), Hash(b2, m2));
}

template<int len>
std::string String<len>::ToString() const {
  return std::string(str_);
}

template<int len>
std::ostream &operator<<(std::ostream &os, const String<len> &str) {
  os << str.str_;
  return os;
}

template<int len>
std::istream &operator>>(std::istream &is, String<len> &str) {
  is >> str.str_;
  return is;
}

template<int len>
int String<len>::Hash(int b, int m) const {
  unsigned long long res = 0;
  for (int i = 0; i < len; i++) {
    res = (res * b + uint8_t(str_[i])) % m;
  }
  return res;
}

}

#endif //TICKET_SYSTEM_STRING_H
