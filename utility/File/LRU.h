#ifndef TICKET_SYSTEM_LRU_H
#define TICKET_SYSTEM_LRU_H

#include "../STLite/list.h"
#include "../STLite/map.h"

namespace bubble {

template<class T, int len> class Storage;

template<class T, int len>
class LRU {
 public:
  explicit LRU(int capacity, Storage<T, len> *file_ptr) : capacity_(capacity), file_(file_ptr), list_(), map_() {}

  bool Contain(int key) const {
    return map_.find(key) != map_.cend();
  }

  T &Fetch(int key) {
    auto &it = map_.find(key)->second;
    list_.push_back(Pair<int, T>(it->first, it->second));
    list_.erase(it);
    it = --list_.end();
    return it->second;
  }

  void Insert(int key, const T &value) {
    list_.push_back(Pair<int, T>(key, value));
    map_.insert(Pair<int, typename List<Pair<int, T>>::iterator>(key, --list_.end()));
    if (list_.size() <= capacity_) {
      return;
    }
    file_->Write(list_.begin()->second, list_.begin()->first, true);
    map_.erase(map_.find(list_.front().first));
    list_.pop_front();
  }

  void Erase(int key) {
    auto it = map_.find(key);
    list_.erase(it->second);
    map_.erase(it);
  }

  void Flush() {
    for (auto it = list_.begin(); it != list_.end(); ++it) {
      file_->Write(it->second, it->first, true);
    }
    list_.clear();
    map_.clear();
  }

 private:
  const int capacity_;
  Storage<T, len> *file_;
  List<Pair<int, T>> list_;
  Map<int, typename List<Pair<int, T>>::iterator> map_;
};

}

#endif //TICKET_SYSTEM_LRU_H
