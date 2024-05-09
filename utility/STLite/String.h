#ifndef TICKET_SYSTEM_STRING_H
#define TICKET_SYSTEM_STRING_H

#include <iostream>
#include <cstring>

namespace bubble {

template<int len>
class String;

template<int len>
std::ostream &operator<<(std::ostream &, const String<len> &);

template<int len>
std::istream &operator>>(std::istream &, String<len> &);

template<int len>
class String {
 public:
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

  friend std::ostream &operator<<<len>(std::ostream &, const String<len> &);

  friend std::istream &operator>><len>(std::istream &, String<len> &);

 private:
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
std::ostream &operator<<(std::ostream &os, const String<len> &str) {
  os << str.str_;
  return os;
}

template<int len>
std::istream &operator>>(std::istream &is, String<len> &str) {
  is >> str.str_;
  return is;
}

}

#endif //TICKET_SYSTEM_STRING_H
