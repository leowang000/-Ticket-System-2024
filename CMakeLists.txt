cmake_minimum_required(VERSION 3.22)
project(ticket_system)

set(CMAKE_CXX_STANDARD 17)

include_directories(src)
include_directories(utility)

add_executable(ticket_system
        src/main.cpp
        src/config.h
        src/ticket_system/TicketSystem.cpp
        src/ticket_system/TicketSystem.h
        src/account/Accounts.cpp
        src/account/Accounts.h
        src/train/Train.cpp
        src/train/Train.h
        src/time/Time.cpp
        src/time/Time.h
        utility/bpt/BPT.h
        utility/File/File.h
        utility/File/File.cpp
        utility/File/LRU.h
        utility/STLite/Vector.h
        utility/STLite/Algorithm.h
        utility/STLite/Pair.h
        utility/STLite/Set.h
        utility/STLite/String.h
        utility/STLite/String.cpp
        utility/STLite/List.h
        utility/STLite/Map.h
        utility/STLite/Array.h)