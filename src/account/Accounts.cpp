#include "Accounts.h"

namespace bubble {

User::User(const username_t &username, const password_t &password, const name_t &name, const mail_t &mail,
           int privilege) :
    username_(username), password_(password), name_(name), mail_(mail), privilege_(privilege) {}

std::string User::ToString() const {
  return username_.ToString() + " " + name_.ToString() + " " + mail_.ToString() + " " + std::to_string(privilege_);
}

Order::Order(int time, const Order::Status &status, const train_id_t &train_id, const station_name_t &from,
             const station_name_t &to, const Date &leave, const Date &arrive, int price, int num) :
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

AccountsManager::AccountsManager(const std::string &user_file_name, const std::string &user_map_file_name,
                                 const std::string &orders_file_name) :
    now_users_(), user_file_(user_file_name), user_map_(user_map_file_name), orders_(orders_file_name) {}

bool AccountsManager::ContainUser(const username_t &username) {
  return !user_map_.Find(username.GetHash()).empty();
}

bool AccountsManager::IsUserLogIn(const username_t &username) {
  return now_users_.find(username.GetHash()) != now_users_.end();
}

void AccountsManager::AddUser(User &user) {
  user_map_.Insert(Pair<HashType, int>(user.username_.GetHash(), user_file_.Write(user)));
}

void AccountsManager::LogIn(const username_t &username) {
  now_users_.insert(username.GetHash());
}

void AccountsManager::LogOut(const username_t &username) {
  now_users_.erase(now_users_.find(username.GetHash()));
}

User AccountsManager::GetUser(const username_t &username) {
  User user;
  user_file_.Read(user, user_map_.Find(username.GetHash()).front());
  return user;
}

User
AccountsManager::Modify(const username_t &username, const password_t &password, const name_t &name, const mail_t &mail,
                        int privilege) {
  static User user;
  int id = user_map_.Find(username.GetHash()).front();
  user_file_.Read(user, id);
  if (password != password_t()) {
    user.password_ = password;
  }
  if (name != name_t()) {
    user.name_ = name;
  }
  if (mail != mail_t()) {
    user.mail_ = mail;
  }
  if (privilege != -1) {
    user.privilege_ = privilege;
  }
  user_file_.Write(user, id);
  return user;
}

Vector<Order> AccountsManager::GetOrder(const username_t &username) {
  return orders_.Find(username.GetHash());
}

void AccountsManager::AddOrder(const username_t &username, const Order &order) {
  orders_.Insert(Pair<HashType, Order>(username.GetHash(), order));
}

void
AccountsManager::ModifyOrderStatus(const username_t &username, const Order &order, const Order::Status &new_status) {
  ModifyOrderStatus(username.GetHash(), order, new_status);
}

void
AccountsManager::ModifyOrderStatus(const HashType &username_hash, const Order &order, const Order::Status &new_status) {
  Order new_order(order);
  new_order.status_ = new_status;
  orders_.Assign(Pair<HashType, Order>(username_hash, order), new_order);
}

void AccountsManager::Clear() {
  now_users_.clear();
  user_file_.clear();
  user_map_.Clear();
  orders_.Clear();
}

}