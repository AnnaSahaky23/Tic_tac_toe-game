⭕ Tic-Tac-Toe in C
A clean, terminal-based two-player Tic-Tac-Toe game built in C using Shared Memory and POSIX Threads for real-time synchronization between two separate terminal windows.   
ZIP

✨ Features
IPC Shared Memory: Allows two independent processes to play on the same live board.   
ZIP

Thread Synchronization: Uses process-shared mutexes and condition variables so turns alternate seamlessly without race conditions.   
ZIP

Robust UI: Clear grid rendering and input validation for smooth gameplay.   
ZIP

🚀 Quick Start
1. Compile the project

Bash
mkdir build && cd build
cmake ..
make
2. Run the game (Open two separate terminals)

Terminal 1 (Player 1 - Creator):

Bash
./TicTacToe player1
Terminal 2 (Player 2 - Joiner):

Bash
./TicTacToe player2
