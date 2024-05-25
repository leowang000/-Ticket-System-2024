#include "String.h"

namespace bubble {

int StringToInt(const std::string &str) {
  int res = 0, len = str.size();
  for (int i = 0; i < len; i++) {
    res = res * 10 + str[i] - '0';
  }
  return res;
}

}
