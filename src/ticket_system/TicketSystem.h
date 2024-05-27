#ifndef TICKET_SYSTEM_TICKETSYSTEM_H
#define TICKET_SYSTEM_TICKETSYSTEM_H

#include "../account/Accounts.h"
#include "../train/Train.h"

namespace bubble {

class TicketSystem {
 public:
  enum InstructionType {
    kAddUser, kLogin, kLogout, kQueryProfile, kModifyProfile, kAddTrain, kDeleteTrain, kReleaseTrain, kQueryTrain,
    kQueryTicket, kQueryTransfer, kBuyTicket, kQueryOrder, kRefundTicket, kClean, kExit
  };

  TicketSystem(const std::string &user_file_name, const std::string &user_map_file_name,
               const std::string &orders_file_name, const std::string &train_file_name,
               const std::string &train_map_file_name, const std::string &seat_file_name,
               const std::string &seat_map_file_name, const std::string &station_map_file_name,
               const std::string &queue_file_name);

  bool GetInstruction();
  bool ExecuteInstruction();

 private:
  struct InstructionParameter {
    InstructionParameter() = default;

    std::string &operator[](char pos);

    int time_;
    Array<std::string, 26> param_;
  };

  AccountsManager accounts_;
  TrainManager trains_;
  InstructionType instruction_type_;
  InstructionParameter param_;
};

}

#endif //TICKET_SYSTEM_TICKETSYSTEM_H
