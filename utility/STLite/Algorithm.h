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

template<class Iter>
Iter MinElement(Iter first, Iter last) {
  if (first == last) {
    return last;
  }
  Iter res = first;
  for (Iter it = ++first; it != last; ++it) {
    if (*it < *res) {
      res = it;
    }
  }
  return res;
}

template<class Iter>
Iter MaxElement(Iter first, Iter last) {
  if (first == last) {
    return last;
  }
  Iter res = first;
  for (Iter it = ++first; it != last; ++it) {
    if (*it > *res) {
      res = it;
    }
  }
  return res;
}

template<class T, class Compare = Less<T>>
T Min(const T &lhs, const T &rhs) {
  return Compare()(lhs, rhs) ? lhs : rhs;
}

template<class T, class Compare = Less<T>>
T Max(const T &lhs, const T &rhs) {
  return Compare()(lhs, rhs) ? rhs : lhs;
}

template<class T, class Compare = Less<T>>
void MergeSort(T *data, int hi, T *buffer, int lo = 0) {
  if (lo + 1 == hi) {
    return;
  }
  int mid = (lo + hi) >> 1;
  MergeSort(data, buffer, lo, mid);
  MergeSort(data, buffer, mid, hi);
  int p1 = lo, p2 = mid, p3 = lo;
  while (p1 < mid && p2 < hi) {
    if (Compare()(data[p1], data[p2])) {
      buffer[p3++] = data[p1++];
    }
    else {
      buffer[p3++] = data[p2++];
    }
  }
  if (p1 == mid) {
    while (p2 < hi) {
      buffer[p3++] = data[p2++];
    }
  }
  else {
    while (p1 < mid) {
      buffer[p3++] = data[p1++];
    }
  }
  for (p3 = lo; p3 < hi; p3++) {
    data[p3] = buffer[p3];
  }
}

}

#endif //TICKET_SYSTEM_ALGORITHM_H
