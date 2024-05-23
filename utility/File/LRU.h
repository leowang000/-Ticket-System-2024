#ifndef TICKET_SYSTEM_LRU_H
#define TICKET_SYSTEM_LRU_H

#include "../STLite/List.h"
#include "../STLite/Map.h"

namespace bubble {

template<class T, int len, bool have_LRU>
class Storage;

template<class T, int len, bool have_LRU>
class LRU {
 public:
  explicit LRU(int capacity, Storage<T, len, have_LRU> *file_ptr);
  ~LRU();

  bool Contain(int key) const;
  T &Fetch(int key);
  void Insert(int key, const T &value);
  void Erase(int key);

 private:
  const int capacity_;
  Storage<T, len, have_LRU> *file_;
  List<Pair<int, T>> list_;
  Map<int, typename List<Pair<int, T>>::iterator> map_;
};

template<class T, int len, bool have_LRU>
LRU<T, len, have_LRU>::LRU(int capacity, Storage<T, len, have_LRU> *file_ptr) :
    capacity_(capacity), file_(file_ptr), list_(), map_() {}

template<class T, int len, bool have_LRU>
LRU<T, len, have_LRU>::~LRU() {
  for (auto it = list_.begin(); it != list_.end(); ++it) {
    file_->Write(it->second, it->first, true);
  }
}

template<class T, int len, bool have_LRU>
bool LRU<T, len, have_LRU>::Contain(int key) const {
  return map_.find(key) != map_.cend();
}

template<class T, int len, bool have_LRU>
T &LRU<T, len, have_LRU>::Fetch(int key) {
  auto &it = map_.find(key)->second;
  list_.push_back(Pair<int, T>(it->first, it->second));
  list_.erase(it);
  it = --list_.end();
  return it->second;
}

template<class T, int len, bool have_LRU>
void LRU<T, len, have_LRU>::Insert(int key, const T &value) {
  list_.push_back(Pair<int, T>(key, value));
  map_.insert(Pair<int, typename List<Pair<int, T>>::iterator>(key, --list_.end()));
  if (list_.size() <= capacity_) {
    return;
  }
  file_->Write(list_.begin()->second, list_.begin()->first, true);
  map_.erase(map_.find(list_.front().first));
  list_.pop_front();
}

template<class T, int len, bool have_LRU>
void LRU<T, len, have_LRU>::Erase(int key) {
  auto it = map_.find(key);
  list_.erase(it->second);
  map_.erase(it);
}

}

#endif //TICKET_SYSTEM_LRU_H
