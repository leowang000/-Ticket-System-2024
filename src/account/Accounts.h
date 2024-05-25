#ifndef TICKET_SYSTEM_ACCOUNTS_H
#define TICKET_SYSTEM_ACCOUNTS_H

#include "../config.h"
#include "../time/Time.h"
#include "../../utility/bpt/BPT.h"
#include "../../utility/File/File.h"
#include "../../utility/STLite/Vector.h"
#include "../../utility/STLite/Set.h"

namespace bubble {

struct User {
  static constexpr int MaxPrivilege = 10;

  User() = default;
  explicit User(const username_t &username, const password_t &password, const name_t &name, const mail_t &mail,
                int privilege);

  std::string ToString() const;

  username_t username_;
  password_t password_;
  name_t name_;
  mail_t mail_;
  int privilege_;
};

struct Order {
  enum Status {
    kSuccess, kPending, kRefunded
  };

  Order() = default;
  explicit Order(int time, const Status &status, const train_id_t &train_id, const station_name_t &from,
                 const station_name_t &to, const Date &leave, const Date &arrive, int price, int num);
  Order(const Order &other) = default;
  Order &operator=(const Order &other) = default;

  std::string ToString() const;
  bool operator<(const Order &rhs) const;
  bool operator==(const Order &rhs) const;
  bool operator>(const Order &rhs) const;
  bool operator!=(const Order &rhs) const;

  int time_, price_, num_;
  Status status_;
  train_id_t train_id_;
  station_name_t from_, to_;
  Date leave_, arrive_;
};

class AccountsManager {
 public:
  AccountsManager(const std::string &user_file_name, const std::string &user_map_file_name,
                  const std::string &orders_file_name);

  bool ContainUser(const username_t &username);
  bool IsUserLogIn(const username_t &username);
  void AddUser(User &user);
  void LogIn(const username_t &username);
  void LogOut(const username_t &username);
  User GetUser(const username_t &username);
  User
  Modify(const username_t &username, const password_t &password, const name_t &name, const mail_t &mail, int privilege);
  Vector<Order> GetOrder(const username_t &username);
  void AddOrder(const username_t &username, const Order &order);
  void ModifyOrderStatus(const username_t &username, const Order &order, const Order::Status &new_status);
  void ModifyOrderStatus(const HashType &username_hash, const Order &order, const Order::Status &new_status);
  void Clear();

 private:
  Set<HashType> now_users_;
  FileWithInt<User> user_file_;
  BPlusTree<HashType, int> user_map_;
  BPlusTree<HashType, Order> orders_;
};

}

#endif //TICKET_SYSTEM_ACCOUNTS_H
