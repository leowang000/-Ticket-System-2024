#include <sstream>

#include "TicketSystem.h"

namespace bubble {

TicketSystem::TicketSystem(const std::string &user_file_name, const std::string &user_map_file_name,
                           const std::string &orders_file_name, const std::string &train_file_name,
                           const std::string &train_map_file_name, const std::string &seat_file_name,
                           const std::string &seat_map_file_name, const std::string &station_map_file_name,
                           const std::string &queue_file_name) :
    trains_(train_file_name, train_map_file_name, seat_file_name, seat_map_file_name,
            station_map_file_name, queue_file_name), accounts_(user_file_name, user_map_file_name,
                                              orders_file_name), instruction_type_(), param_() {}

std::string &TicketSystem::InstructionParameter::operator[](char ch) {
  return param_[ch - 'a'];
}

bool TicketSystem::GetInstruction() {
  std::string line, str;
  if (!getline(std::cin, line)) {
    return false;
  }
  std::stringstream sstr(line);
  char ch;
  sstr >> ch >> param_.time_ >> ch >> str;
  if (str == "add_user") {
    instruction_type_ = kAddUser;
  }
  else if (str == "login") {
    instruction_type_ = kLogin;
  }
  else if (str == "logout") {
    instruction_type_ = kLogout;
  }
  else if (str == "query_profile") {
    instruction_type_ = kQueryProfile;
  }
  else if (str == "modify_profile") {
    instruction_type_ = kModifyProfile;
    param_['p'].clear();
    param_['n'].clear();
    param_['m'].clear();
    param_['g'].clear();
  }
  else if (str == "add_train") {
    instruction_type_ = kAddTrain;
  }
  else if (str == "delete_train") {
    instruction_type_ = kDeleteTrain;
  }
  else if (str == "release_train") {
    instruction_type_ = kReleaseTrain;
  }
  else if (str == "query_train") {
    instruction_type_ = kQueryTrain;
  }
  else if (str == "query_ticket") {
    instruction_type_ = kQueryTicket;
    param_['p'].clear();
  }
  else if (str == "query_transfer") {
    instruction_type_ = kQueryTransfer;
  }
  else if (str == "buy_ticket") {
    instruction_type_ = kBuyTicket;
    param_['q'].clear();
  }
  else if (str == "query_order") {
    instruction_type_ = kQueryOrder;
  }
  else if (str == "refund_ticket") {
    instruction_type_ = kRefundTicket;
    param_['n'].clear();
  }
  else if (str == "clean") {
    instruction_type_ = kClean;
  }
  else if (str == "exit") {
    instruction_type_ = kExit;
  }
  while (sstr >> ch >> ch >> str) {
    param_[ch] = str;
  }
  return true;
}

bool TicketSystem::ExecuteInstruction() {
  std::cout << "[" + std::to_string(param_.time_) + "] ";
  switch (instruction_type_) {
    case kAddUser: {
      User user{username_t(param_['u']), password_t(param_['p']), name_t(param_['n']), mail_t(param_['m']),
                accounts_.Empty() ? 10 : StringToInt(param_['g'])};
      if (accounts_.Empty()) {
        accounts_.AddUser(user);
        std::cout << "0\n";
        break;
      }
      if (accounts_.IsUserLogIn(username_t(param_['c'])) && !accounts_.ContainUser(user.username_)) {
        User cur = accounts_.GetUser(username_t(param_['c']));
        if (cur.privilege_ > user.privilege_) {
          accounts_.AddUser(user);
          std::cout << "0\n";
          break;
        }
      }
      std::cout << "-1\n";
      break;
    }
    case kLogin: {
      if (accounts_.ContainUser(username_t(param_['u'])) && !accounts_.IsUserLogIn(username_t(param_['u']))) {
        User user = accounts_.GetUser(username_t(param_['u']));
        if (user.password_ == password_t(param_['p'])) {
          accounts_.LogIn(username_t(param_['u']));
          std::cout << "0\n";
          break;
        }
      }
      std::cout << "-1\n";
      break;
    }
    case kLogout: {
      if (accounts_.IsUserLogIn(username_t(param_['u']))) {
        accounts_.LogOut(username_t(param_['u']));
        std::cout << "0\n";
      }
      else {
        std::cout << "-1\n";
      }
      break;
    }
    case kQueryProfile: {
      if (accounts_.IsUserLogIn(username_t(param_['c'])) && accounts_.ContainUser(username_t(param_['u']))) {
        User cur = accounts_.GetUser(username_t(param_['c']));
        User user = accounts_.GetUser(username_t(param_['u']));
        if (cur.privilege_ > user.privilege_ || param_['c'] == param_['u']) {
          std::cout << user.ToString() << "\n";
          break;
        }
      }
      std::cout << "-1\n";
      break;
    }
    case kModifyProfile: {
      if (accounts_.IsUserLogIn(username_t(param_['c'])) && accounts_.ContainUser(username_t(param_['u']))) {
        User cur = accounts_.GetUser(username_t(param_['c']));
        User user = accounts_.GetUser(username_t(param_['u']));
        int new_privilege = param_['g'].empty() ? -1 : StringToInt(param_['g']);
        if ((cur.privilege_ > user.privilege_ || param_['c'] == param_['u']) && cur.privilege_ > new_privilege) {
          std::cout << accounts_.Modify(username_t(param_['u']), password_t(param_['p']), name_t(param_['n']),
                                        mail_t(param_['m']), new_privilege).ToString() << "\n";
          break;
        }
      }
      std::cout << "-1\n";
      break;
    }
    case kAddTrain: {
      if (!trains_.ContainTrain(train_id_t(param_['i']))) {
        Train train{train_id_t(param_['i']), StringToInt(param_['n']), StringToInt(param_['m']),
                    Array<int, Train::MaxStationNum - 1>(param_['p']),
                    Array<station_name_t, Train::MaxStationNum>(param_['s']), Time(param_['x']),
                    Array<int, Train::MaxStationNum - 1>(param_['t']),
                    Array<int, Train::MaxStationNum - 2>(param_['o']), Date(param_['d'].substr(0, 5)),
                    Date(param_['d'].substr(6, 5)), param_['y'][0]};
        trains_.AddTrain(train);
        std::cout << "0\n";
      }
      else {
        std::cout << "-1\n";
      }
      break;
    }
    case kDeleteTrain: {
      if (trains_.ContainTrain(train_id_t(param_['i'])) && !trains_.IsTrainReleased(train_id_t(param_['i']))) {
        trains_.DeleteTrain(train_id_t(param_['i']));
        std::cout << "0\n";
      }
      else {
        std::cout << "-1\n";
      }
      break;
    }
    case kReleaseTrain: {
      if (trains_.ContainTrain(train_id_t(param_['i'])) && !trains_.IsTrainReleased(train_id_t(param_['i']))) {
        trains_.ReleaseTrain(train_id_t(param_['i']));
        std::cout << "0\n";
      }
      else {
        std::cout << "-1\n";
      }
      break;
    }
    case kQueryTrain: {
      if (trains_.ContainTrain(train_id_t(param_['i']))) {
        std::string str = trains_.GetTrainInfo(train_id_t(param_['i']), Date(param_['d']));
        if (!str.empty()) {
          std::cout << str;
          break;
        }
      }
      std::cout << "-1\n";
      break;
    }
    case kQueryTicket: {
      if (param_['p'].empty()) {
        param_['p'] = "time";
      }
      auto vec(trains_.GetTicket(Date(param_['d']), station_name_t(param_['s']), station_name_t(param_['t'])));
      std::cout << vec.size() << "\n";
      struct TimeCmp {
        bool operator()(const TicketInfo &lhs, const TicketInfo &rhs) const {
          int lhs_time = Date::Diff(lhs.arrive_, lhs.leave_).GetMinutes();
          int rhs_time = Date::Diff(rhs.arrive_, rhs.leave_).GetMinutes();
          if (lhs_time != rhs_time) {
            return lhs_time < rhs_time;
          }
          return lhs.train_id_ < rhs.train_id_;
        }
      };
      struct PriceCmp {
        bool operator()(const TicketInfo &lhs, const TicketInfo &rhs) const {
          if (lhs.price_ != rhs.price_) {
            return lhs.price_ < rhs.price_;
          }
          return lhs.train_id_ < rhs.train_id_;
        }
      };
      if (param_['p'] == "time") {
        Sort(vec.data(), vec.data() + vec.size(), TimeCmp());
      }
      else {
        Sort(vec.data(), vec.data() + vec.size(), PriceCmp());
      }
      for (const auto &ticket_info: vec) {
        std::cout << ticket_info.ToString() << "\n";
      }
      break;
    }
    case kQueryTransfer: {
      if (param_['p'].empty()) {
        param_['p'] = "time";
      }
      auto vec(trains_.GetTransfer(Date(param_['d']), station_name_t(param_['s']), station_name_t(param_['t'])));
      if (vec.empty()) {
        std::cout << "0\n";
        break;
      }
      struct TimeCmp {
        bool operator()(const Pair<TicketInfo, TicketInfo> &lhs, const Pair<TicketInfo, TicketInfo> &rhs) const {
          int lhs_time = Date::Diff(lhs.second.arrive_, lhs.first.leave_).GetMinutes();
          int rhs_time = Date::Diff(rhs.second.arrive_, rhs.first.leave_).GetMinutes();
          int lhs_price = lhs.first.price_ + lhs.second.price_;
          int rhs_price = rhs.first.price_ + rhs.second.price_;
          if (lhs_time != rhs_time) {
            return lhs_time < rhs_time;
          }
          if (lhs_price != rhs_price) {
            return lhs_price < rhs_price;
          }
          if (lhs.first.train_id_ != rhs.first.train_id_) {
            return lhs.first.train_id_ < rhs.first.train_id_;
          }
          return lhs.second.train_id_ < rhs.second.train_id_;
        }
      };
      struct PriceCmp {
        bool operator()(const Pair<TicketInfo, TicketInfo> &lhs, const Pair<TicketInfo, TicketInfo> &rhs) const {
          int lhs_time = Date::Diff(lhs.second.arrive_, lhs.first.leave_).GetMinutes();
          int rhs_time = Date::Diff(rhs.second.arrive_, rhs.first.leave_).GetMinutes();
          int lhs_price = lhs.first.price_ + lhs.second.price_;
          int rhs_price = rhs.first.price_ + rhs.second.price_;
          if (lhs_price != rhs_price) {
            return lhs_price < rhs_price;
          }
          if (lhs_time != rhs_time) {
            return lhs_time < rhs_time;
          }
          if (lhs.first.train_id_ != rhs.first.train_id_) {
            return lhs.first.train_id_ < rhs.first.train_id_;
          }
          return lhs.second.train_id_ < rhs.second.train_id_;
        }
      };
      Pair<TicketInfo, TicketInfo> *res;
      if (param_['p'] == "time") {
        res = MinElement(vec.data(), vec.data() + vec.size(), TimeCmp());
      }
      else {
        res = MinElement(vec.data(), vec.data() + vec.size(), PriceCmp());
      }
      std::cout << res->first.ToString() << "\n" << res->second.ToString() << "\n";
      break;
    }
    case kBuyTicket: {
      if (param_['q'].empty()) {
        param_['q'] = "false";
      }
      if (accounts_.IsUserLogIn(username_t(param_['u'])) && trains_.ContainTrain(train_id_t(param_['i'])) &&
          trains_.IsTrainReleased(train_id_t(param_['i']))) {
        Order order(
            trains_.BuyTicket(param_.time_, train_id_t(param_['i']), Date(param_['d']), StringToInt(param_['n']),
                              station_name_t(param_['f']), station_name_t(param_['t'])));
        if (order.time_ != 0) {
          if (order.status_ == Order::kSuccess) {
            accounts_.AddOrder(username_t(param_['u']), order);
            std::cout << static_cast<unsigned long long>(order.price_) * order.num_ << "\n";
            break;
          }
          if (param_['q'] == "true") {
            accounts_.AddOrder(username_t(param_['u']), order);
            trains_.EnQueue(username_t(param_['u']), order);
            std::cout << "queue\n";
            break;
          }
        }
      }
      std::cout << "-1\n";
      break;
    }
    case kQueryOrder: {
      if (accounts_.IsUserLogIn(username_t(param_['u']))) {
        auto vec(accounts_.GetOrder(username_t(param_['u'])));
        std::cout << vec.size() << "\n";
        for (int i = vec.size() - 1; i >= 0; i--) {
          std::cout << vec[i].ToString() << "\n";
        }
        break;
      }
      std::cout << "-1\n";
      break;
    }
    case kRefundTicket: {
      if (param_['n'].empty()) {
        param_['n'] = "1";
      }
      if (accounts_.IsUserLogIn(username_t(param_['u']))) {
        int n = StringToInt(param_['n']);
        auto vec(accounts_.GetOrder(username_t(param_['u'])));
        if (n <= vec.size() && vec[vec.size() - n].status_ != Order::kRefunded) {
          accounts_.ModifyOrderStatus(username_t(param_['u']), vec[vec.size() - n], Order::kRefunded);
          trains_.RefundTicket(vec[vec.size() - n], accounts_);
          std::cout << "0\n";
          break;
        }
      }
      std::cout << "-1\n";
      break;
    }
    case kClean: {
      accounts_.Clear();
      trains_.Clear();
      std::cout << "0\n";
      break;
    }

    case kExit: {
      std::cout << "bye\n";
      return true;
    }
  }
  return false;
}

}