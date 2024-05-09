#ifndef TICKET_SYSTEM_PAIR_H
#define TICKET_SYSTEM_PAIR_H

namespace bubble {

template<class T1, class T2>
class Pair {
 public:
  struct CompareByKey {
    bool operator()(const Pair &lhs, const Pair &rhs) const {
      return lhs.first < rhs.first;
    }
  };

  constexpr Pair() : first(), second() {}

  Pair(const Pair &other) = default;

  Pair(Pair &&other) = default;

  Pair(const T1 &x, const T2 &y) : first(x), second(y) {}

  template<class U1, class U2>
  Pair(U1 &&x, U2 &&y) : first(x), second(y) {}

  template<class U1, class U2>
  Pair(const Pair<U1, U2> &other) : first(other.first), second(other.second) {}

  template<class U1, class U2>
  Pair(Pair<U1, U2> &&other) : first(other.first), second(other.second) {}

  Pair &operator=(const Pair &other) = default;

  bool operator<(const Pair &rhs) const {
    return first < rhs.first || (first == rhs.first && second < rhs.second);
  }

  bool operator==(const Pair &rhs) const {
    return first == rhs.first && second == rhs.second;
  }

  bool operator!=(const Pair &rhs) const {
    return first != rhs.first || second != rhs.second;
  }

  T1 first;
  T2 second;
};

}

#endif //TICKET_SYSTEM_PAIR_H
