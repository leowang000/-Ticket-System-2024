#ifndef TICKET_SYSTEM_FILE_H
#define TICKET_SYSTEM_FILE_H

#include <fstream>
#include <filesystem>
#include <string>

#include "LRU.h"
#include "../STLite/Set.h"
#include "../STLite/Vector.h"

namespace bubble {

class File : public std::fstream {
 public:
  using PtrType = int;
  static constexpr int PageSize = 4096;

  explicit File(const std::string &file_name);

  void OpenOrCreate();
  bool IsCreated() const;
  void OpenAndClear();
  void Clear();
  template<class T> void Read(T &dst, PtrType ptr);
  template<class T> void ReadRange(T *dst, PtrType ptr, int n);
  template<class T> void Write(T &src, PtrType ptr);
  template<class T> void WriteRange(T *src, PtrType ptr, int n);
  int FileSize() const;

 public:
  std::string file_name_;
};

template<class T, int len = 0>
class FileWithInt : public File {
 public:
  explicit FileWithInt(const std::string &file_name);

  void Read(T &dst, int id); // 1-base
  void ReadRange(T *dst, int n, int id);
  int Write(T &src, int id = 0); // 1-base
  int WriteRange(T *src, int n, int id = 0);
  void ReadInt(int &dst, int id); // 1-base
  void WriteInt(int &src, int id); // 1-base
  int DataCount() const;
};

template<class T, int len = 0, bool have_buffer = true>
class Storage {
 public:
  explicit Storage(const std::string &file_name, int buffer_pool_size = BufferPoolSize);
  ~Storage();

  void Read(T &dst, int id);
  int Write(T &src, int id = 0, bool write_to_file = false);
  void Erase(int id);
  void ReadInt(int &dst, int id);
  void WriteInt(int &src, int id);
  bool IsCreated();
  void Clear();

 private:
  static constexpr int BufferPoolSize = 384;
  FileWithInt<T, len> data_file_;
  FileWithInt<int> trash_file_;
  LRU<T, len> *buffer;
  Set<int> pool_;
  int last_id_;
};

template<class T>
void File::Read(T &dst, PtrType ptr) {
  OpenOrCreate();
  seekg(ptr);
  read(reinterpret_cast<char *>(&dst), sizeof(T));
  close();
}

template<class T>
void File::ReadRange(T *dst, PtrType ptr, int n) {
  OpenOrCreate();
  seekg(ptr);
  read(reinterpret_cast<char *>(dst), sizeof(T) * n);
  close();
}

template<class T>
void File::Write(T &src, PtrType ptr) {
  OpenOrCreate();
  seekp(ptr);
  write(reinterpret_cast<char *>(&src), sizeof(T));
  close();
}

template<class T>
void File::WriteRange(T *src, PtrType ptr, int n) {
  OpenOrCreate();
  seekp(ptr);
  write(reinterpret_cast<char *>(src), sizeof(T) * n);
  close();
}

template<class T, int len>
FileWithInt<T, len>::FileWithInt(const std::string &file_name) : File(file_name) {}

template<class T, int len>
void FileWithInt<T, len>::Read(T &dst, int id) { // 1-base
  File::Read(dst, len * sizeof(int) + sizeof(T) * (id - 1));
}

template<class T, int len>
void FileWithInt<T, len>::ReadRange(T *dst, int n, int id) {
  File::ReadRange(dst, len * sizeof(int) + sizeof(T) * (id - 1), n);
}

template<class T, int len>
int FileWithInt<T, len>::Write(T &src, int id) { // 1-base
  if (id == 0) {
    id = DataCount() + 1;
  }
  File::Write(src, len * sizeof(int) + sizeof(T) * (id - 1));
  return id;
}

template<class T, int len>
int FileWithInt<T, len>::WriteRange(T *src, int n, int id) {
  if (id == 0) {
    id = DataCount() + 1;
  }
  File::WriteRange(src, len * sizeof(int) + sizeof(T) * (id - 1), n);
  return id;
}

template<class T, int len>
void FileWithInt<T, len>::ReadInt(int &dst, int id) { // 1-base
  File::Read(dst, sizeof(int) * (id - 1));
}

template<class T, int len>
void FileWithInt<T, len>::WriteInt(int &src, int id) { // 1-base
  File::Write(src, sizeof(int) * (id - 1));
}

template<class T, int len>
int FileWithInt<T, len>::DataCount() const { // 1-base
  return IsCreated() ? (FileSize() - len * sizeof(int)) / sizeof(T) : 0;
}

template<class T, int len, bool have_buffer>
Storage<T, len, have_buffer>::Storage(const std::string &file_name, int buffer_pool_size) :
    data_file_(file_name + "_data"), trash_file_(file_name + "_trash"), pool_() {
  if constexpr (have_buffer) {
    buffer = new LRU<T, len>(buffer_pool_size, this);
  }
  else {
    buffer = nullptr;
  }
  if (data_file_.IsCreated()) {
    last_id_ = data_file_.DataCount();
    trash_file_.OpenOrCreate();
    int cnt = trash_file_.DataCount(), dst[File::PageSize / sizeof(int)], i;
    for (i = 1; i + File::PageSize / sizeof(int) - 1 <= cnt; i += File::PageSize / sizeof(int)) {
      trash_file_.ReadRange(dst, File::PageSize / sizeof(int), i);
      for (int id: dst) {
        pool_.insert(id);
      }
    }
    if (i <= cnt) {
      trash_file_.ReadRange(dst, cnt - i + 1, i);
      for (int j = 0; j < cnt - i + 1; j++) {
        pool_.insert(dst[j]);
      }
    }
    trash_file_.close();
  }
  else {
    last_id_ = 0;
  }
}

template<class T, int len, bool have_buffer>
Storage<T, len, have_buffer>::~Storage() {
  delete buffer;
  trash_file_.OpenAndClear();
  int src[File::PageSize / sizeof(int)], cnt = 0;
  for (auto item : pool_) {
    src[cnt++] = item;
    if (cnt == File::PageSize / sizeof(int)) {
      trash_file_.write(reinterpret_cast<char *>(&src), File::PageSize);
      cnt = 0;
    }
  }
  if (cnt != 0) {
    trash_file_.write(reinterpret_cast<char *>(&src), cnt * sizeof(int));
  }
  trash_file_.close();
}

template<class T, int len, bool have_buffer>
void Storage<T, len, have_buffer>::Read(T &dst, int id) {
  if constexpr (have_buffer) {
    if (buffer->Contain(id)) {
      dst = buffer->Fetch(id);
      return;
    }
    data_file_.Read(dst, id);
    buffer->Insert(id, dst);
  }
  else {
    data_file_.Read(dst, id);
  }
}

template<class T, int len, bool have_buffer>
int Storage<T, len, have_buffer>::Write(T &src, int id, bool write_to_file) {
  if constexpr (have_buffer) {
    if (buffer->Contain(id) && !write_to_file) {
      buffer->Fetch(id) = src;
    }
    else if (id == 0) {
      if (pool_.empty()) {
        id = ++last_id_;
      }
      else {
        id = *pool_.begin();
        pool_.erase(pool_.begin());
      }
      buffer->Insert(id, src);
    }
    else {
      data_file_.Write(src, id);
    }
    return id;
  }
  else {
    if (id == 0) {
      if (pool_.empty()) {
        id = ++last_id_;
      }
      else {
        id = *pool_.begin();
        pool_.erase(pool_.begin());
      }
    }
    data_file_.Write(src, id);
    return id;
  }
}

template<class T, int len, bool have_buffer>
void Storage<T, len, have_buffer>::Erase(int id) {
  pool_.insert(id);
  if constexpr (have_buffer) {
    if (buffer->Contain(id)) {
      buffer->Erase(id);
    }
  }
}

template<class T, int len, bool have_buffer>
void Storage<T, len, have_buffer>::ReadInt(int &dst, int id) {
  data_file_.ReadInt(dst, id);
}

template<class T, int len, bool have_buffer>
void Storage<T, len, have_buffer>::WriteInt(int &src, int id) {
  data_file_.WriteInt(src, id);
}

template<class T, int len, bool have_buffer>
bool Storage<T, len, have_buffer>::IsCreated() {
  return data_file_.IsCreated();
}

template<class T, int len, bool have_buffer>
void Storage<T, len, have_buffer>::Clear() {
  data_file_.Clear();
  trash_file_.Clear();
  pool_.clear();
  last_id_ = 0;
  if constexpr (have_buffer) {
    buffer->Clear();
  }
}

}

#endif //TICKET_SYSTEM_FILE_H