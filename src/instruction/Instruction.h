#ifndef TICKET_SYSTEM_INSTRUCTION_H
#define TICKET_SYSTEM_INSTRUCTION_H

#include "../account/Accounts.h"
#include "../train/Train.h"

namespace bubble {

struct Instruction {
  Instruction() = default;
  virtual bool Check() const = 0;
  virtual bool Execute(AccountsManager &accounts, TrainManager &trains) = 0;
};

struct AddUserInstruction : public Instruction {
 public:
  AddUserInstruction();

  bool Check();
  bool Execute(AccountsManager &accounts, TrainManager &trains);

 private:

};

struct LogInInstruction : public Instruction {
 public:
  LogInInstruction();

  bool Check();
  bool Execute(AccountsManager &accounts, TrainManager &trains);

 private:

};

struct LogOutInstruction : public Instruction {
 public:
  LogOutInstruction();

  bool Check();
  bool Execute(AccountsManager &accounts, TrainManager &trains);

 private:

};

struct QueryProfileInstruction : public Instruction {
 public:
  QueryProfileInstruction();

  bool Check();
  bool Execute(AccountsManager &accounts, TrainManager &trains);

 private:

};

struct ModifyProfileInstruction : public Instruction {
 public:
  ModifyProfileInstruction();

  bool Check();
  bool Execute(AccountsManager &accounts, TrainManager &trains);

 private:

};

struct AddTrainInstruction : public Instruction {
 public:
  AddTrainInstruction();

  bool Check();
  bool Execute(AccountsManager &accounts, TrainManager &trains);

 private:

};

struct DeleteTrainInstruction : public Instruction {
 public:
  DeleteTrainInstruction();

  bool Check();
  bool Execute(AccountsManager &accounts, TrainManager &trains);

 private:

};

struct ReleaseTrainInstruction : public Instruction {
 public:
  ReleaseTrainInstruction();

  bool Check();
  bool Execute(AccountsManager &accounts, TrainManager &trains);

 private:

};

struct QueryTrainInstruction : public Instruction {
 public:
  QueryTrainInstruction();

  bool Check();
  bool Execute(AccountsManager &accounts, TrainManager &trains);

 private:

};

struct QueryTicketInstruction : public Instruction {
 public:
  QueryTicketInstruction();

  bool Check();
  bool Execute(AccountsManager &accounts, TrainManager &trains);

 private:

};

struct QueryTransferInstruction : public Instruction {
 public:
  QueryTransferInstruction();

  bool Check();
  bool Execute(AccountsManager &accounts, TrainManager &trains);

 private:

};

struct BuyTicketInstruction : public Instruction {
 public:
  BuyTicketInstruction();

  bool Check();
  bool Execute(AccountsManager &accounts, TrainManager &trains);

 private:

};

struct QueryOrderInstruction : public Instruction {
 public:
  QueryOrderInstruction();

  bool Check();
  bool Execute(AccountsManager &accounts, TrainManager &trains);

 private:

};

struct RefundTicketInstruction : public Instruction {
 public:
  RefundTicketInstruction();

  bool Check();
  bool Execute(AccountsManager &accounts, TrainManager &trains);

 private:

};

struct CleanInstruction : public Instruction {
 public:
  CleanInstruction();

  bool Check();
  bool Execute(AccountsManager &accounts, TrainManager &trains);

 private:

};

struct ExitInstruction : public Instruction {
 public:
  ExitInstruction();

  bool Check();
  bool Execute(AccountsManager &accounts, TrainManager &trains);

 private:

};

}

#endif //TICKET_SYSTEM_INSTRUCTION_H
