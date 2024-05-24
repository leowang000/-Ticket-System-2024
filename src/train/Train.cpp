#include "Train.h"

namespace bubble {

Train::Train(const train_id_t &train_id, int station_num, int seat_num, const Array<int, MaxStationNum - 1> &prices,
             const Array<station_name_t, MaxStationNum> &stations, const Time &start_time,
             const Array<int, MaxStationNum - 1> &travel_times, const Array<int, MaxStationNum - 2> &stop_over_times,
             const Date &begin, const Date &end, char type) :
    train_id_(train_id), station_num_(station_num), seat_num_(
    seat_num), price_(), station_names_(), arrive_(), leave_(), begin_date_(begin), end_date_(end), type_(
    type), released_(false) {
  price_[0] = 0;
  for (int i = 1; i < station_num; i++) {
    price_[i] = price_[i - 1] + prices[i - 1];
  }
  for (int i = 0; i < station_num; i++) {
    station_names_[i] = stations[i];
  }
  leave_[0] = start_time;
  for (int i = 1; i <= station_num - 2; i++) {
    arrive_[i] = leave_[i - 1] + Time(travel_times[i - 1]);
    leave_[i] = arrive_[i] + Time(stop_over_times[i]);
  }
  arrive_[station_num - 1] = leave_[station_num - 2] + Time(travel_times[station_num - 2]);
}

//std::string Train::ToString(const Date &date, TrainManager &trains) const {
//  std::string res;
//  res += train_id_.ToString() + " " + type_ + "\n";
//  for (int i = 0; i < station_num_; i++) {
//    res += station_names_[i].ToString() + " " + (i == 0 ? "xx-xx xx:xx" : (date + arrive_[i]).ToString()) + " -> " +
//           (i == station_num_ - 1 ? "xx-xx xx:xx" : (date + leave_[i]).ToString()) + " " + std::to_string(price_[i]) +
//           " " + (i == station_num_ - 1 ? "x" : std::to_string(
//        trains.GetSeatNum(train_id_, Date::Diff(date, begin_date_).day_, i))) + "\n";
//  }
//}

TicketInfo::TicketInfo(const train_id_t &train_id, int seat, int price, const station_name_t &from,
                       const station_name_t &to, const Date &leave, const Date &arrive) :
    train_id_(train_id), seat_(seat), price_(price), from_(from), to_(to), leave_(leave), arrive_(arrive) {}

std::string TicketInfo::ToString() const {
  return train_id_.ToString() + " " + from_.ToString() + " " + leave_.ToString() + " -> " + to_.ToString() + " " +
         arrive_.ToString() + std::to_string(price_) + " " + std::to_string(seat_);
}

bool TrainManager::ContainTrain(const train_id_t &train_id) {
  return !train_map_.Find(train_id.GetHash()).empty();
}

bool TrainManager::IsTrainReleased(const bubble::train_id_t &train_id) {
  Train train;
  train_file_.Read(train, train_map_.Find(train_id.GetHash()).front());
  return train.released_;
}

void TrainManager::AddTrain(Train &train) {
  train_map_.Insert(Pair<HashType, int>(train.train_id_.GetHash(), train_file_.Write(train)));
}

void TrainManager::DeleteTrain(const train_id_t &train_id) {
  HashType train_hash = train_id.GetHash();
  int id = train_map_.Find(train_hash).front();
  train_file_.Erase(id);
  train_map_.Erase(Pair<HashType, int>(train_hash, id));
}

void TrainManager::ReleaseTrain(const train_id_t &train_id) {
  static Seat buffer[MaxDateNum];
  HashType train_hash = train_id.GetHash();
  Train train;
  int id = train_map_.Find(train_hash).front();
  train_file_.Read(train, id);
  train.released_ = true;
  train_file_.Write(train, id);
  int date_num = Date::Diff(train.end_date_, train.begin_date_).day_;
  for (int j = 0; j < date_num; j++) {
    for (int i = 0; i < train.station_num_; i++) {
      buffer[j][i] = train.seat_num_;
    }
  }
  seat_map_.Insert(Pair<HashType, int>(train_hash, seat_file_.WriteRange(buffer, date_num)));
  for (int i = 0; i < train.station_num_; i++) {
    station_map_.Insert(
        Pair<HashType, Pair<HashType, int>>(train.station_names_[i].GetHash(), Pair<HashType, int>(train_hash, i)));
  }
}

Train TrainManager::GetTrain(const train_id_t &train_id) {
  Train train;
  train_file_.Read(train, train_map_.Find(train_id.GetHash()).front());
  return train;
}

Vector<TicketInfo> TrainManager::GetTicket(const Date &date, const station_name_t &from, const station_name_t &to) {
  Vector<TicketInfo> res;
  Vector<Pair<HashType, int>> from_trains(station_map_.Find(from.GetHash()));
  Vector<Pair<HashType, int>> to_trains(station_map_.Find(to.GetHash()));
  int from_ptr = 0, to_ptr = 0;
  while (from_ptr < from_trains.size() && to_ptr < to_trains.size()) {
    if (from_trains[from_ptr].first == to_trains[to_ptr].first) {
      HashType train_hash = from_trains[from_ptr].first;
      int from_id = from_trains[from_ptr].second, to_id = to_trains[to_ptr].second;
      if (from_id < to_id) {
        Train train;
        train_file_.Read(train, train_map_.Find(train_hash).front());
        res.push_back(GetFirstTicket(train, date, from_id, to_id));
      }
      from_ptr++;
      to_ptr++;
    }
    else if (from_trains[from_ptr].first < to_trains[to_ptr].first) {
      from_ptr++;
    }
    else {
      to_ptr++;
    }
  }
  return res;
}

Vector<Pair<TicketInfo, TicketInfo>>
TrainManager::GetTransfer(const Date &date, const station_name_t &from, const station_name_t &to) {
  static Train from_train, to_train;
  static HashType from_train_station_hash[Train::MaxStationNum], to_train_station_hash[Train::MaxStationNum];
  Vector<Pair<TicketInfo, TicketInfo>> res;
  Vector<Pair<HashType, int>> from_trains(station_map_.Find(from.GetHash()));
  Vector<Pair<HashType, int>> to_trains(station_map_.Find(to.GetHash()));
  for (auto &from_info: from_trains) {
    for (auto &to_info: to_trains) {
      if (from_info.first == to_info.first) {
        continue;
      }
      train_file_.Read(from_train, train_map_.Find(from_info.first).front());
      train_file_.Read(to_train, train_map_.Find(to_info.first).front());
      for (int i = from_info.second + 1; i < from_train.station_num_; i++) {
        from_train_station_hash[i] = from_train.station_names_[i].GetHash();
      }
      for (int j = 0; j < to_info.second; j++) {
        to_train_station_hash[j] = to_train.station_names_[j].GetHash();
      }
      bool found = false;
      for (int i = from_info.second + 1; !found && i < from_train.station_num_; i++) {
        for (int j = 0; j < to_info.second; j++) {
          if (from_train_station_hash[i] == to_train_station_hash[j]) {
            TicketInfo ticket_from = GetFirstTicket(from_train, date, from_info.second, i);
            int offset = GetOffset(from_train, date, from_info.second);
            TicketInfo ticket_to = GetFirstTicket(to_train,
                                                  from_train.begin_date_ + Time(offset, 0, 0) + from_train.arrive_[i],
                                                  j, to_info.second);
            if (ticket_to.train_id_ != train_id_t()) {
              found = true;
              res.push_back(Pair<TicketInfo, TicketInfo>(ticket_from, ticket_to));
              break;
            }
          }
        }
      }
    }
  }
  return res;
}

int TrainManager::BuyTicket(const train_id_t &train_id, const Date &date, int num, const station_name_t &from,
                            const station_name_t &to) {
  static Train train;
  HashType train_hash = train_id.GetHash();
  train_file_.Read(train, train_map_.Find(train_hash).front());
  int from_id, to_id;
  GetFromAndToId(train, from, to, from_id, to_id);
  int offset = GetOffset(train, date, from_id);
  static Seat buffer;
  int seat_id = seat_map_.Find(train_hash).front() + offset;
  seat_file_.Read(buffer, seat_id);
  if (*MinElement(buffer + from_id, buffer + to_id) < num) {
    return 0;
  }
  for (int i = from_id; i < to_id; i++) {
    buffer[i] -= num;
  }
  seat_file_.Write(buffer, seat_id);
  return train.price_[to_id] - train.price_[from_id];
}

void TrainManager::RefundTicket(const Order &order, AccountsManager &accounts) {
  static Train train;
  if (order.status_ != Order::kSuccess) {
    return;
  }
  HashType train_hash = order.train_id_.GetHash();
  train_file_.Read(train, train_map_.Find(train_hash).front());
  int from_id, to_id;
  GetFromAndToId(train, order.from_, order.to_, from_id, to_id);
  int offset = Date::Diff(order.leave_, train.begin_date_ + train.leave_[from_id]).day_;
  static Seat buffer;
  int seat_id = seat_map_.Find(train_hash).front() + offset;
  seat_file_.Read(buffer, seat_id);
  for (int i = from_id; i < to_id; i++) {
    buffer[i] += order.num_;
  }
  auto it = queue_.begin();
  while (it != queue_.end()) {
    if (it->second.train_id_ != train.train_id_) {
      ++it;
      continue;
    }
    GetFromAndToId(train, it->second.from_, it->second.to_, from_id, to_id);
    if (*MinElement(buffer + from_id, buffer + to_id) < it->second.num_) {
      ++it;
      continue;
    }
    for (int i = from_id; i < to_id; i++) {
      buffer[i] -= it->second.num_;
    }
    accounts.ModifyOrderStatus(it->first, it->second, Order::kSuccess);
    queue_.erase(it++);
  }
  seat_file_.Write(buffer, seat_id);
}

void TrainManager::EnQueue(const username_t &username, const Order &order) {
  queue_.push_back(Pair<HashType, Order>(username.GetHash(), order));
}

int TrainManager::GetSeatNum(const bubble::train_id_t &train_id, int offset, int station_id) {
  static Seat buffer;
  seat_file_.Read(buffer, seat_map_.Find(train_id.GetHash()).front() + offset);
  return buffer[station_id];
}

void TrainManager::Clear() {
  train_file_.Clear();
  train_map_.Clear();
  seat_file_.Clear();
  seat_map_.Clear();
  queue_.clear();
  station_map_.Clear();
}

int TrainManager::GetOffset(const Train &train, const Date &date, int id) {
  if (Date::Diff(train.end_date_ + train.leave_[id], date).day_ < 0) {
    return -1;
  }
  Time diff = Date::Diff(date, train.begin_date_ + train.leave_[id]);
  return Max((diff.hour_ == 0 && diff.min_ == 0 ? diff.day_ : diff.day_ + 1), 0);
}

void
TrainManager::GetFromAndToId(const Train &train, const station_name_t &from, const station_name_t &to, int &from_id,
                             int &to_id) {
  from_id = -1;
  for (int i = 0; i < train.station_num_; i++) {
    if (from_id == -1 && train.station_names_[i] == from) {
      from_id = i;
    }
    if (from_id != -1 && train.station_names_[i] == to) {
      to_id = i;
      return;
    }
  }
}

int TrainManager::GetSeatNum(const train_id_t &train_id, int offset, int from_id, int to_id) {
  static Seat buffer;
  seat_file_.Read(buffer, seat_map_.Find(train_id.GetHash()).front() + offset);
  return *MinElement(buffer + from_id, buffer + to_id);
}

TicketInfo TrainManager::GetFirstTicket(const Train &train, const Date &date, int from_id, int to_id) {
  int price = train.price_[to_id] - train.price_[from_id];
  int offset = GetOffset(train, date, from_id);
  if (offset == -1) {
    return TicketInfo();
  }
  Date start_date = train.begin_date_ + Time(offset, 0, 0);
  Date leave = start_date + train.leave_[from_id], arrive = start_date + train.arrive_[to_id];
  int seat = GetSeatNum(train.train_id_, offset, from_id, to_id);
  return TicketInfo(train.train_id_, seat, price, train.station_names_[from_id], train.station_names_[to_id], leave,
                    arrive);
}

}