cmake_minimum_required(VERSION 3.22)
project(ticket_system)

set(CMAKE_CXX_STANDARD 17)

include_directories(src)
include_directories(utility)

add_executable(code
        src/main.cpp
        src/ticket_system/TicketSystem.cpp
        src/account/Accounts.cpp
        src/train/Train.cpp
        src/time/Time.cpp
        utility/File/File.cpp
        utility/STLite/String.cpp)