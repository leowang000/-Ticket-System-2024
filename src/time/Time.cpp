#include <string>

#include "Time.h"
#include "../utility/STLite/String.h"

namespace bubble {

Time::Time(int m, int d, int hr, int mi) : m_(m), d_(d), hr_(hr), mi_(mi) {}

std::string Time::ToString(bool require_date, bool require_time) const {
  if (m_ == 0 && d_ == 0 && hr_ == 0 && mi_ == 0) {
    return "xx-xx xx:xx";
  }
  std::string res;
  if (require_date) {
    res += bubble::ToString(m_, 2) + "-" + bubble::ToString(d_, 2);
    if (require_time) {
      res += " ";
    }
  }
  if (require_time) {
    res += bubble::ToString(hr_, 2) + ":" + bubble::ToString(mi_, 2);
  }
  return res;
}

Time Time::operator+(const Time &rhs) const {
  Time res(m_ + rhs.m_, d_ + rhs.d_, hr_ + rhs.hr_, mi_ + rhs.mi_);
  if (res.mi_ >= 60) {
    res.hr_ += res.mi_ / 60;
    res.mi_ %= 60;
  }
  if (res.hr_ >= 24) {
    res.d_ += res.hr_ / 24;
    res.hr_ %= 24;
  }
  while (res.d_ > DayOfMonth[res.m_]) {
    res.d_ -= DayOfMonth[res.m_];
    res.m_++;
  }
  return res;
}

}