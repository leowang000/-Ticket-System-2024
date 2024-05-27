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

template<class Iter, class T>
Iter LowerBound(Iter first, Iter last, const T &value) {
  if (first == last) {
    return last;
  }
  Iter lo = first, hi = last - 1;
  if (*hi < value) {
    return last;
  }
  while (lo != hi) {
    Iter mid = lo + ((hi - lo) >> 1);
    if (*mid < value) {
      lo = mid + 1;
    }
    else {
      hi = mid;
    }
  }
  return lo;
}

template<class Iter, class T, class Compare>
Iter LowerBound(Iter first, Iter last, const T &value, Compare cmp) {
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

template<class Iter, class T>
Iter UpperBound(Iter first, Iter last, const T &value) {
  if (first == last) {
    return last;
  }
  Iter lo = first, hi = last - 1;
  if (!(value < *hi)) {
    return last;
  }
  while (lo != hi) {
    Iter mid = lo + ((hi - lo) >> 1);
    if (value < *mid) {
      hi = mid;
    }
    else {
      lo = mid + 1;
    }
  }
  return lo;
}

template<class Iter, class T, class Compare>
Iter UpperBound(Iter first, Iter last, const T &value, Compare cmp) {
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

template<class Iter, class Compare>
Iter MinElement(Iter first, Iter last, Compare cmp) {
  if (first == last) {
    return last;
  }
  Iter res = first;
  for (Iter it = ++first; it != last; ++it) {
    if (cmp(*it, *res)) {
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

template<class Iter, class Compare>
Iter MaxElement(Iter first, Iter last, Compare cmp) {
  if (first == last) {
    return last;
  }
  Iter res = first;
  for (Iter it = ++first; it != last; ++it) {
    if (cmp(res, *it)) {
      res = it;
    }
  }
  return res;
}

template<class T>
T Min(const T &lhs, const T &rhs) {
  return lhs < rhs ? lhs : rhs;
}

template<class T, class Compare>
T Min(const T &lhs, const T &rhs, Compare cmp) {
  return cmp(lhs, rhs) ? lhs : rhs;
}

template<class T>
T Max(const T &lhs, const T &rhs) {
  return lhs < rhs ? rhs : lhs;
}

template<class T, class Compare>
T Max(const T &lhs, const T &rhs, Compare cmp) {
  return cmp(lhs, rhs) ? rhs : lhs;
}

template<class T>
void Sort(T *first, T *last) {
  if (first + 1 == last) {
    return;
  }
  int lo = 0, hi = last - first - 1, mid = (lo + hi) / 2;
  T pivot = first[mid];
  while (lo < hi) {
    if (lo < mid) {
      if (pivot < first[lo]) {
        first[mid] = first[lo];
        mid = lo;
      }
      else {
        lo++;
      }
    }
    if (hi > mid) {
      if (first[hi] < pivot) {
        first[mid] = first[hi];
        mid = hi;
      }
      else {
        hi--;
      }
    }
  }
  first[mid] = pivot;
  Sort(first, first + mid);
  Sort(first + mid + 1, last);
}

template<class T, class Compare>
void Sort(T *first, T *last, Compare cmp) {
  if (first == last || first + 1 == last) {
    return;
  }
  int lo = 0, hi = last - first - 1, mid = (lo + hi) / 2;
  T pivot = first[mid];
  while (lo < hi) {
    if (lo < mid) {
      if (cmp(pivot, first[lo])) {
        first[mid] = first[lo];
        mid = lo;
      }
      else {
        lo++;
      }
    }
    if (hi > mid) {
      if (cmp(first[hi], pivot)) {
        first[mid] = first[hi];
        mid = hi;
      }
      else {
        hi--;
      }
    }
  }
  first[mid] = pivot;
  Sort(first, first + mid, cmp);
  Sort(first + mid + 1, last, cmp);
}

}

#endif //TICKET_SYSTEM_ALGORITHM_H
