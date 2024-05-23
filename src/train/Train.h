#ifndef TICKET_SYSTEM_TRAIN_H
#define TICKET_SYSTEM_TRAIN_H

#include "../config.h"
#include "../time/Time.h"
#include "../../utility/bpt/BPT.h"
#include "../../utility/File/File.h"

namespace bubble {

struct Train {
  static constexpr int MaxStationNum = 100;

  train_id_t train_id_;
  int station_num_, seat_num_, price_[MaxStationNum];
  station_name_t station_names_[MaxStationNum];
  Time arrive_[MaxStationNum], leave_[MaxStationNum], begin_date, end_date;
};

using Seat = int[Train::MaxStationNum - 1];

struct TicketInfo {
  train_id_t train_id_;
  int seat_, price_;
  station_name_t from_, to_;
  Time arrive_, leave_;
};

class TrainManager {
 public:
  bool ContainTrain(const Train &train);
  bool AddTrain(const Train &train);
  bool DeleteTrain(const train_id_t &train_id);
  bool ReleaseTrain(const train_id_t &train_id);
  Train GetTrain(const train_id_t &train_id);
  Vector<TicketInfo> GetTicket(const Time &time, const station_name_t &from, const station_name_t &to);
  Pair<TicketInfo, TicketInfo> GetTransfer(const station_name_t &from, const station_name_t &to);
  int BuyTicket(const train_id_t &train_id, const Time &time, int num, const station_name_t &from,
                const station_name_t &to);

 private:
  Storage<Train, 0, false> train_file_;
  BPlusTree<int, int> train_map_;
  FileWithInt<Seat> seat_file_;
  BPlusTree<int, int> seat_map_;
  BPlusTree<int, Pair<int, int>> station_map_;
};

}

#endif //TICKET_SYSTEM_TRAIN_H
