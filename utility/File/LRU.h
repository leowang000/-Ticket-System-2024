#ifndef TICKET_SYSTEM_LRU_H
#define TICKET_SYSTEM_LRU_H

#include "../STLite/List.h"
#include "../STLite/Map.h"

namespace bubble {

template<class T, int len, bool have_buffer>
class Storage;

template<class T, int len>
class LRU {
 public:
  explicit LRU(int capacity, Storage<T, len, true> *file_ptr);
  ~LRU();

  bool Contain(int key) const;
  T &Fetch(int key);
  void Insert(int key, const T &value);
  void Erase(int key);
  void Clear();

 private:
  const int capacity_;
  Storage<T, len, true> *file_;
  List<Pair<int, T>> list_;
  Map<int, typename List<Pair<int, T>>::iterator> map_;
};

template<class T, int len>
LRU<T, len>::LRU(int capacity, Storage<T, len, true> *file_ptr) :
    capacity_(capacity), file_(file_ptr), list_(), map_() {}

template<class T, int len>
LRU<T, len>::~LRU() {
  for (auto it = list_.begin(); it != list_.end(); ++it) {
    file_->Write(it->second, it->first, true);
  }
}

template<class T, int len>
bool LRU<T, len>::Contain(int key) const {
  return map_.find(key) != map_.cend();
}

template<class T, int len>
T &LRU<T, len>::Fetch(int key) {
  auto &it = map_.find(key)->second;
  list_.replace(it, list_.end());
  return it->second;
}

template<class T, int len>
void LRU<T, len>::Insert(int key, const T &value) {
  list_.push_back(Pair<int, T>(key, value));
  map_.insert(Pair<int, typename List<Pair<int, T>>::iterator>(key, --list_.end()));
  if (list_.size() <= capacity_) {
    return;
  }
  file_->Write(list_.begin()->second, list_.begin()->first, true);
  map_.erase(map_.find(list_.front().first));
  list_.pop_front();
}

template<class T, int len>
void LRU<T, len>::Erase(int key) {
  auto it = map_.find(key);
  list_.erase(it->second);
  map_.erase(it);
}

template<class T, int len>
void LRU<T, len>::Clear() {
  list_.clear();
  map_.clear();
}

}

#endif //TICKET_SYSTEM_LRU_H
