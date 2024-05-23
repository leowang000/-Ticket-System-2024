#ifndef TICKET_SYSTEM_TICKET_SYSTEM_H
#define TICKET_SYSTEM_TICKET_SYSTEM_H

#include "../instruction/Instruction.h"

namespace bubble {

struct Instruction;

class TicketSystem {
 public:
  void GetInstruction();
  void CheckInstruction();
  bool ExecuteInstruction();

 private:
  Instruction *inst_;
  AccountsManager accounts_;
  TrainManager trains_;
};

}

#endif //TICKET_SYSTEM_TICKET_SYSTEM_H
