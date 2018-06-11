cmake_minimum_required(VERSION 3.10)
project(BTrees)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)

add_executable(BTrees
        include/Block.hpp
        include/BTree.hpp
        include/BTree.inl
        include/Commands.hpp
        include/Entry.hpp
        src/Commands.cpp
        src/main.cpp include/IdealBTree.hpp)
