cmake_minimum_required(VERSION 3.22)
project(ticket_system)

set(CMAKE_CXX_STANDARD 17)

include_directories(src)
include_directories(utility)

add_executable(ticket_system
        src/main.cpp
        utility/bpt/BPT.h
        utility/File/File.h
        utility/File/LRU.h
        utility/STLite/Vector.h
        utility/STLite/Algorithm.h
        utility/STLite/Pair.h
        utility/STLite/Set.h
        utility/STLite/String.h
        utility/STLite/List.h
        utility/STLite/Map.h
        utility/STLite/Array.h
        src/time/Time.cpp
        src/time/Time.h
        src/account/Accounts.cpp
        src/account/Accounts.h
        src/config.h
        src/train/Train.cpp
        src/train/Train.h
        src/instruction/Instruction.cpp
        src/instruction/Instruction.h
        src/ticket_system/ticket_system.cpp
        src/ticket_system/ticket_system.h)