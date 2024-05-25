#include <string>

#include "Time.h"
#include "../utility/STLite/String.h"
#include "../utility/STLite/Algorithm.h"

namespace bubble {

Time::Time(int d, int hr, int mi) : day_(d), hour_(hr), min_(mi) {}

Time::Time(int n) : day_(n / 1440), hour_(n % 1440 / 60), min_(n % 60) {}

Time::Time(const std::string &str) : day_() {
  hour_ = (str[0] - '0') * 10 + str[1] - '0';
  min_ = (str[3] - '0') * 10 + str[4] - '0';
}

Time Time::operator+(const Time &rhs) const {
  return Time(GetMinutes() + rhs.GetMinutes());
}

Time Time::operator-(const Time &rhs) const {
  return Time(GetMinutes() - rhs.GetMinutes());
}

constexpr int Time::GetMinutes() const {
  return 1440 * day_ + 60 * hour_ + min_;
}

Date::Date(int m, int d, int hr, int mi) : m_(m), d_(d), hr_(hr), mi_(mi) {}

Date::Date(const std::string &str) {
  m_ = (str[0] - '0') * 10 + str[1] - '0';
  d_ = (str[3] - '0') * 10 + str[4] - '0';
  if (str.size() == 11) {
    hr_ = (str[6] - '0') * 10 + str[7] - '0';
    mi_ = (str[9] - '0') * 10 + str[10] - '0';
  }
  else {
    hr_ = 0;
    mi_ = 0;
  }
}

std::string Date::ToString(bool require_date, bool require_time) const {
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

Date Date::operator+(const Time &rhs) const {
  Date res(m_, d_ + rhs.day_, hr_ + rhs.hour_, mi_ + rhs.min_);
  if (res.mi_ >= 60) {
    res.mi_ -= 60;
    res.hr_ ++;
  }
  if (res.hr_ >= 24) {
    res.hr_ -= 24;
    res.d_ ++;
  }
  while (res.d_ > DayOfMonth[res.m_]) {
    res.d_ -= DayOfMonth[res.m_];
    res.m_++;
  }
  return res;
}

Date Date::operator-(const Time &rhs) const {
  int d = SumDayOfMonth[m_ - 1] + d_ - 1 - rhs.day_, hr = hr_ - rhs.hour_, mi = mi_ - rhs.min_;
  if (mi < 0) {
    mi += 60;
    hr--;
  }
  if (hr < 0) {
    hr += 24;
    d--;
  }
  int m = *UpperBound(DayOfMonth, DayOfMonth + 13, d);
  return Date(m, d + 1 - DayOfMonth[m - 1], hr, mi);
}

Time Date::Diff(const Date &date1, const Date &date2) {
  int d1 = SumDayOfMonth[date1.m_ - 1] + date1.d_ - 1, d2 = SumDayOfMonth[date2.m_ - 1] + date2.d_ - 1;
  int d = d2 - d1, hr = date2.hr_ - date1.hr_, mi = date2.mi_ - date1.mi_;
  if (mi < 0) {
    mi += 60;
    hr--;
  }
  if (hr < 0) {
    hr += 24;
    d--;
  }
  return Time(d, hr, mi);
}

}