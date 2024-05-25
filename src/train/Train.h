#ifndef TICKET_SYSTEM_TRAIN_H
#define TICKET_SYSTEM_TRAIN_H

#include "../config.h"
#include "../time/Time.h"
#include "../../utility/bpt/BPT.h"
#include "../../utility/File/File.h"
#include "../../utility/STLite/List.h"
#include "../../utility/STLite/Array.h"
#include "../account/Accounts.h"

namespace bubble {

class TrainManager;

struct Train {
  static constexpr int MaxStationNum = 100;

  Train() = default;
  Train(const train_id_t &train_id, int station_num, int seat_num, const Array<int, MaxStationNum - 1> &prices,
        const Array<station_name_t, MaxStationNum> &stations, const Time &start_time,
        const Array<int, MaxStationNum - 1> &travel_times, const Array<int, MaxStationNum - 2> &stop_over_times,
        const Date &begin, const Date &end, char type);

  train_id_t train_id_;
  int station_num_, seat_num_, price_[MaxStationNum];
  station_name_t station_names_[MaxStationNum];
  Time arrive_[MaxStationNum], leave_[MaxStationNum];
  Date begin_date_, end_date_;
  char type_;
  bool released_;
};

struct TicketInfo {
  TicketInfo() = default;
  TicketInfo(const train_id_t &train_id, int seat, int price, const station_name_t &from, const station_name_t &to,
             const Date &leave, const Date &arrive);

  std::string ToString() const;

  train_id_t train_id_;
  int seat_, price_;
  station_name_t from_, to_;
  Date leave_, arrive_;
};

class TrainManager {
 public:
  using Seat = int[Train::MaxStationNum - 1];

  static constexpr int MaxDateNum = 92;

  TrainManager(const std::string &train_file_name, const std::string &train_map_file_name,
               const std::string &seat_file_name, const std::string &seat_map_file_name,
               const std::string &station_map_file_name);

  bool ContainTrain(const train_id_t &train_id);
  bool IsTrainReleased(const train_id_t &train_id);
  void AddTrain(Train &train);
  void DeleteTrain(const train_id_t &train_id);
  void ReleaseTrain(const train_id_t &train_id);
  std::string GetTrainInfo(const train_id_t &train_id, const Date &date);
  Vector<TicketInfo> GetTicket(const Date &date, const station_name_t &from, const station_name_t &to);
  Vector<Pair<TicketInfo, TicketInfo>>
  GetTransfer(const Date &date, const station_name_t &from, const station_name_t &to);
  Order BuyTicket(int time, const train_id_t &train_id, const Date &date, int num, const station_name_t &from,
                const station_name_t &to);
  void RefundTicket(const Order &order, AccountsManager &accounts);
  void EnQueue(const username_t &username, const Order &order);
  void Clear();

 private:
  static int GetOffset(const Train &train, const Date &date, int id);
  static void
  GetFromAndToId(const Train &train, const station_name_t &from, const station_name_t &to, int &from_id, int &to_id);
  int GetSeatNum(const train_id_t &train_id, int offset, int from_id, int to_id);
  TicketInfo GetFirstTicket(const Train &train, const Date &date, int from_id, int to_id);

  Storage<Train, 0, false> train_file_;
  BPlusTree<HashType, int> train_map_;
  FileWithInt<Seat> seat_file_;
  BPlusTree<HashType, int> seat_map_;
  BPlusTree<HashType, Pair<HashType, int>> station_map_;
  List<Pair<HashType, Order>> queue_;
};

}

#endif //TICKET_SYSTEM_TRAIN_H
