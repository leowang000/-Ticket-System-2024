#ifndef TICKET_SYSTEM_TIME_H
#define TICKET_SYSTEM_TIME_H

namespace bubble {

class Time {
 public:
  explicit Time(int m = 0, int d = 0, int hr = 0, int mi = 0);

  std::string ToString(bool require_date = true, bool require_time = true) const;
  Time operator+(const Time &rhs) const;

 private:
  static constexpr int DayOfMonth[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

  int m_, d_, hr_, mi_;
};

}

#endif //TICKET_SYSTEM_TIME_H
