cmake_minimum_required(VERSION 3.16)
project(TicTacToe C)

add_executable(TicTacToe game.c)

target_link_libraries(TicTacToe PRIVATE pthread)