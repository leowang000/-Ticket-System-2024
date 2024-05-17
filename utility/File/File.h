#ifndef TICKET_SYSTEM_FILE_H
#define TICKET_SYSTEM_FILE_H

#include <fstream>
#include <filesystem>
#include <string>

#include "LRU.h"
#include "../STLite/set.h"
#include "../STLite/vector.h"

namespace bubble {

class File : public std::fstream {
 public:
  using PtrType = int;

  static constexpr int kBlockSize = 4096;

  explicit File(const std::string &file_name) : std::fstream(), file_name_(file_name) {}

  void OpenOrCreate() {
    open(file_name_);
    if (good()) {
      return;
    }
    open(file_name_, std::ios::out);
    close();
    open(file_name_);
  }

  bool IsCreated() {
    open(file_name_);
    if (good()) {
      close();
      return true;
    }
    return false;
  }

  void OpenAndClear() {
    open(file_name_, std::ios::out);
  }

  template<class T>
  void Read(T &dst, PtrType ptr) {
    OpenOrCreate();
    seekg(ptr);
    read(reinterpret_cast<char *>(&dst), sizeof(T));
    close();
  }

  template<class T>
  void ReadRange(T *dst, PtrType ptr, int n) {
    OpenOrCreate();
    seekg(ptr);
    read(reinterpret_cast<char *>(dst), sizeof(T) * n);
    close();
  }

  template<class T>
  void Write(T &src, PtrType ptr) {
    OpenOrCreate();
    seekp(ptr);
    write(reinterpret_cast<char *>(&src), sizeof(T));
    close();
  }

  int FileSize() const {
    return std::filesystem::file_size(file_name_);
  }

 private:
  std::string file_name_;
};

template<class T, int len = 0>
class FileWithInt : public File {
 public:
  explicit FileWithInt(const std::string &file_name) : File(file_name) {}

  void Read(T &dst, int id) { // 1-base
    File::Read(dst, len * sizeof(int) + sizeof(T) * (id - 1));
  }

  void ReadRange(T *dst, int id, int n) {
    File::ReadRange(dst, len * sizeof(int) + sizeof(T) * (id - 1), n);
  }

  void Write(T &src, int id) { // 1-base
    File::Write(src, len * sizeof(int) + sizeof(T) * (id - 1));
  }

  void ReadInt(int &dst, int id) { // 1-base
    File::Read(dst, sizeof(int) * (id - 1));
  }

  void WriteInt(int &src, int id) { // 1-base
    File::Write(src, sizeof(int) * (id - 1));
  }
};

template<class T, int len>
class Storage {
 public:
  Storage(const std::string &data_file_name, const std::string &trash_file_name) :
      data_file_(data_file_name), trash_file_(trash_file_name), pool_(), buffer(BufferPoolSize, this) {
    if (data_file_.IsCreated()) {
      last_id_ = (data_file_.FileSize() - len) / sizeof(T);
      trash_file_.OpenOrCreate();
      int cnt = trash_file_.FileSize() / sizeof(int), dst[File::kBlockSize / sizeof(int)], i;
      for (i = 1; i + File::kBlockSize / sizeof(int) - 1 <= cnt; i += File::kBlockSize / sizeof(int)) {
        trash_file_.ReadRange(dst, i, File::kBlockSize / sizeof(int));
        for (int id : dst) {
          pool_.insert(id);
        }
      }
      if (i <= cnt) {
        trash_file_.ReadRange(dst, i, cnt - i + 1);
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

  ~Storage() {
    buffer.Flush();
    trash_file_.OpenAndClear();
    int src[File::kBlockSize / sizeof(int)], cnt = 0;
    while (!pool_.empty()) {
      src[cnt++] = *pool_.begin();
      pool_.erase(pool_.begin());
      if (cnt == File::kBlockSize / sizeof(int)) {
        trash_file_.write(reinterpret_cast<char *>(&src), File::kBlockSize);
        cnt = 0;
      }
    }
    if (cnt != 0) {
      trash_file_.write(reinterpret_cast<char *>(&src), cnt * sizeof(int));
    }
    trash_file_.close();
  }

  void Read(T &dst, int id) {
    if (buffer.Contain(id)) {
      dst = buffer.Fetch(id);
      return;
    }
    data_file_.Read(dst, id);
    buffer.Insert(id, dst);
  }

  int Write(T &src, int id = 0, bool write_to_file = false) {
    if (buffer.Contain(id) && !write_to_file) {
      buffer.Fetch(id) = src;
    }
    else if (id == 0) {
      if (pool_.empty()) {
        id = ++last_id_;
      }
      else {
        id = *pool_.begin();
        pool_.erase(pool_.begin());
      }
      buffer.Insert(id, src);
    }
    else {
      data_file_.Write(src, id);
    }
    return id;
  }

  void Erase(int id) {
    pool_.insert(id);
    if (buffer.Contain(id)) {
      buffer.Erase(id);
    }
  }

  void ReadInt(int &dst, int id) {
    data_file_.ReadInt(dst, id);
  }

  void WriteInt(int &src, int id) {
    data_file_.WriteInt(src, id);
  }

  bool IsCreated() {
    return data_file_.IsCreated();
  }

 private:
  static constexpr int BufferPoolSize = 1024;
  LRU<T, len> buffer;
  FileWithInt<T, len> data_file_;
  FileWithInt<int> trash_file_;
  Set<int> pool_;
  int last_id_;
};

}

#endif //TICKET_SYSTEM_FILE_H