// Ticket-System-2024
#include <iostream>
#include <filesystem>

#include "ticket_system/TicketSystem.h"

const std::string user_file_name = "user";
const std::string user_map_file_name = "user_map";
const std::string orders_file_name = "orders";
const std::string train_file_name = "train";
const std::string train_map_file_name = "train_map";
const std::string seat_file_name = "seat";
const std::string seat_map_file_name = "seat_map";
const std::string station_map_file_name = "station_map";
const std::string queue_file_name = "queue";

void ClearFiles() {
  std::filesystem::remove(user_file_name);
  std::filesystem::remove(user_map_file_name + "_leaf_data");
  std::filesystem::remove(user_map_file_name + "_leaf_trash");
  std::filesystem::remove(user_map_file_name + "_node_data");
  std::filesystem::remove(user_map_file_name + "_node_trash");
  std::filesystem::remove(orders_file_name + "_leaf_data");
  std::filesystem::remove(orders_file_name + "_leaf_trash");
  std::filesystem::remove(orders_file_name + "_node_data");
  std::filesystem::remove(orders_file_name + "_node_trash");
  std::filesystem::remove(train_file_name + "_data");
  std::filesystem::remove(train_file_name + "_trash");
  std::filesystem::remove(train_map_file_name + "_leaf_data");
  std::filesystem::remove(train_map_file_name + "_leaf_trash");
  std::filesystem::remove(train_map_file_name + "_node_data");
  std::filesystem::remove(train_map_file_name + "_node_trash");
  std::filesystem::remove(seat_file_name);
  std::filesystem::remove(seat_map_file_name + "_leaf_data");
  std::filesystem::remove(seat_map_file_name + "_leaf_trash");
  std::filesystem::remove(seat_map_file_name + "_node_data");
  std::filesystem::remove(seat_map_file_name + "_node_trash");
  std::filesystem::remove(station_map_file_name + "_leaf_data");
  std::filesystem::remove(station_map_file_name + "_leaf_trash");
  std::filesystem::remove(station_map_file_name + "_node_data");
  std::filesystem::remove(station_map_file_name + "_node_trash");
  std::filesystem::remove(queue_file_name);
}

int main() {
  //ClearFiles();
  //freopen("../testcases/44.in","r", stdin);
  //freopen("output.txt", "w", stdout);
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::cout.tie(nullptr);
  bubble::TicketSystem ticket_system(user_file_name, user_map_file_name, orders_file_name, train_file_name,
                                     train_map_file_name, seat_file_name, seat_map_file_name, station_map_file_name,
                                     queue_file_name);
  while (ticket_system.GetInstruction()) {
    if (ticket_system.ExecuteInstruction()) {
      return 0;
    }
  }
  return 0;
}