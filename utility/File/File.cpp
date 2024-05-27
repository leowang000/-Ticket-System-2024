#include "File.h"

namespace bubble {

File::File(const std::string &file_name) : std::fstream(), file_name_(file_name) {}

void File::OpenOrCreate() {
  open(file_name_);
  if (good()) {
    return;
  }
  open(file_name_, std::ios::out);
  close();
  open(file_name_);
}

bool File::IsCreated() const {
  return std::filesystem::exists(file_name_);
}

void File::OpenAndClear() {
  open(file_name_, std::ios::out);
}

void File::Clear() {
  open(file_name_, std::ios::out);
  close();
}

int File::FileSize() const {
  return IsCreated() ? std::filesystem::file_size(file_name_) : 0;
}

}