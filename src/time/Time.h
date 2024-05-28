#ifndef TICKET_SYSTEM_TIME_H
#define TICKET_SYSTEM_TIME_H

namespace bubble {

struct Time {
  Time() = default;
  explicit Time(int n);
  Time(int d, int hr, int mi);
  explicit Time(const std::string &str);

  Time operator+(const Time &rhs) const;
  Time operator-(const Time &rhs) const;
  constexpr int GetMinutes() const;

  int day_, hour_, min_;
};

class Date {
 public:
  struct CompareByDay {
    bool operator()(const Date &lhs, const Date &rhs) const;
  };

  struct CompareByTime {
    bool operator()(const Date &lhs, const Date &rhs) const;
  };

  Date() = default;
  Date(int m, int d, int hr, int mi);
  explicit Date(const std::string &str);

  std::string ToString(bool require_date = true, bool require_time = true) const;
  Date operator+(const Time &rhs) const;
  Date operator-(const Time &rhs) const;
  static Time Diff(const Date &date1, const Date &date2);

 private:
  static constexpr int DayOfMonth[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  static constexpr int SumDayOfMonth[13] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};

  int m_, d_, hr_, mi_;
};

}

#endif //TICKET_SYSTEM_TIME_H