#ifndef TICKET_SYSTEM_ALGORITHM_H
#define TICKET_SYSTEM_ALGORITHM_H

namespace bubble {

template<class T>
struct Less {
  bool operator()(const T &lhs, const T &rhs) const {
    return lhs < rhs;
  }
};

template<class T>
class Greater {
  bool operator()(const T &lhs, const T &rhs) const {
    return rhs < lhs;
  }
};

template<class Iter, class T, class Compare = Less<T>>
Iter LowerBound(Iter first, Iter last, const T &value, Compare cmp = Compare()) {
  if (first == last) {
    return last;
  }
  Iter lo = first, hi = last - 1;
  if (cmp(*hi, value)) {
    return last;
  }
  while (lo != hi) {
    Iter mid = lo + ((hi - lo) >> 1);
    if (cmp(*mid, value)) {
      lo = mid + 1;
    }
    else {
      hi = mid;
    }
  }
  return lo;
}

template<class Iter, class T, class Compare = Less<T>>
Iter UpperBound(Iter first, Iter last, const T &value, Compare cmp = Compare()) {
  if (first == last) {
    return last;
  }
  Iter lo = first, hi = last - 1;
  if (!cmp(value, *hi)) {
    return last;
  }
  while (lo != hi) {
    Iter mid = lo + ((hi - lo) >> 1);
    if (cmp(value, *mid)) {
      hi = mid;
    }
    else {
      lo = mid + 1;
    }
  }
  return lo;
}

}

#endif //TICKET_SYSTEM_ALGORITHM_H
