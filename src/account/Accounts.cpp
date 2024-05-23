#include "Accounts.h"

namespace bubble {

User::User(const username_t &username, const password_t &password, const name_t &name, const mail_t &mail,
           int privilege) :
    username_(username), password_(password), name_(name), mail_(mail), privilege_(privilege) {}

Order::Order(int time, const Order::Status &status, const train_id_t &train_id, const station_name_t &from,
             const station_name_t &to, const Time &leave, const Time &arrive, int price, int num) :
    time_(time), status_(status), train_id_(train_id), from_(from), to_(to), leave_(leave), arrive_(arrive), price_(
    price), num_(num) {}

std::string Order::ToString() const {
  std::string status_string;
  switch (status_) {
    case kSuccess:
      status_string = "[success]";
      break;
    case kPending:
      status_string = "[pending]";
      break;
    case kRefunded:
      status_string = "[refunded]";
      break;
  }
  return status_string + " " + train_id_.ToString() + " " + from_.ToString() + " " + leave_.ToString() + " -> " +
         to_.ToString() + " " + arrive_.ToString() + " " + std::to_string(price_) + " " + std::to_string(num_);
}

bool Order::operator<(const Order &rhs) const {
  return time_ < rhs.time_;
}

bool Order::operator==(const Order &rhs) const {
  return time_ == rhs.time_;
}

bool Order::operator>(const Order &rhs) const {
  return time_ > rhs.time_;
}

bool Order::operator!=(const Order &rhs) const {
  return time_ != rhs.time_;
}

AccountsManager::AccountsManager(const std::string &user_file_name, const std::string &now_users_file_name,
                   const std::string &orders_file_name) :
    now_users_(), user_file_(user_file_name), users_(now_users_file_name), orders_(orders_file_name), queue_() {}

}