#ifndef TICKET_SYSTEM_ACCOUNTS_H
#define TICKET_SYSTEM_ACCOUNTS_H

#include "../config.h"
#include "../time/Time.h"
#include "../../utility/bpt/BPT.h"
#include "../../utility/File/File.h"
#include "../../utility/STLite/List.h"
#include "../../utility/STLite/Vector.h"
#include "../../utility/STLite/Set.h"

namespace bubble {

struct User {
  static constexpr int MaxPrivilege = 10;

  username_t username_;
  password_t password_;
  name_t name_;
  mail_t mail_;
  int privilege_;

  explicit User(const username_t &username = username_t(), const password_t &password = password_t(),
                const name_t &name = name_t(), const mail_t &mail = mail_t(), int privilege = MaxPrivilege);
};

struct Order {
  enum Status {
    kSuccess, kPending, kRefunded
  };

  int time_, price_, num_;
  Status status_;
  train_id_t train_id_;
  station_name_t from_, to_;
  Time leave_, arrive_;

  explicit Order(int time, const Status &status = Status(), const train_id_t &train_id = train_id_t(),
                 const station_name_t &from = station_name_t(), const station_name_t &to = station_name_t(),
                 const Time &leave = Time(), const Time &arrive = Time(), int price = 0, int num = 0);

  std::string ToString() const;
  bool operator<(const Order &rhs) const;
  bool operator==(const Order &rhs) const;
  bool operator>(const Order &rhs) const;
  bool operator!=(const Order &rhs) const;
};

class AccountsManager {
 public:
  AccountsManager(const std::string &user_file_name, const std::string &now_users_file_name,
                  const std::string &orders_file_name);

  bool ContainUser(const username_t &username);
  bool AddUser(const User &user);
  bool LogIn(const username_t &username);
  bool LogOut(const username_t &username);
  User GetUser(const username_t &username);
  void Modify(const username_t &username, const password_t &password = password_t(), const name_t &name = name_t(),
              const mail_t &mail = mail_t(), int privilege = User::MaxPrivilege);
  Vector<Order> GetOrder(const username_t &username);
  void AddOrder(const Order &order);
  void RefundOrder(const Order &order);
  void Clear();

 private:
  Set<int> now_users_;
  FileWithInt<User> user_file_;
  BPlusTree<int, int> users_;
  BPlusTree<int, Order> orders_;
  List<Pair<int, int>> queue_;
};

}

#endif //TICKET_SYSTEM_ACCOUNTS_H
