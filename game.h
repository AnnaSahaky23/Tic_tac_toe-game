#pragma once
#include <pthread.h>

#define size 3

typedef struct {
    char board[size][size]; // ' ', 'X', or 'O'
    int turn;               // 1 = player1, 2 = player2
    int move_count;
    char winner;            // 'X', 'O', or '-'
    int game_over;          // 1 = game ended
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} SharedGame;

#define SHM_KEY 12345  // System V shared memory key
