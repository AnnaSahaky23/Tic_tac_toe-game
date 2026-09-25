#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <errno.h>

// Attach or create shared memory
SharedGame* create_and_map_shared_memory(int is_creator);

// Draws the board to terminal
void draw_board(char board[size][size]);

// Checks if the given player has won
int check_win(char board[size][size], char sym);

int main(int argc, char* argv[]) {
    if (argc != 2 || (strcmp(argv[1], "player1") && strcmp(argv[1], "player2"))) {
        fprintf(stderr, "Usage: %s player1|player2\n", argv[0]);
        return 1;
    }

    int is_player1 = strcmp(argv[1], "player1") == 0;
    SharedGame* game = create_and_map_shared_memory(is_player1);
    if (!game) return 1;

    char symbol = is_player1 ? 'X' : 'O';
    int my_turn = is_player1 ? 1 : 2;

    while (1) {
        pthread_mutex_lock(&game->mutex);

        while (game->turn != my_turn && !game->game_over)
            pthread_cond_wait(&game->cond, &game->mutex);

        if (game->game_over) {
            pthread_mutex_unlock(&game->mutex);
            break;
        }

        printf("=====================================\n");
        printf("It's Player %d's turn (%c)\n", my_turn, symbol);
        draw_board(game->board);

        int row, col, valid = 0;
        while (!valid) {
            printf("Enter your move (row col): ");
            if (scanf("%d %d", &row, &col) != 2) {
                while (getchar() != '\n'); // clear invalid input
                printf("Invalid input! Try again.\n");
                continue;
            }

            if (row < 0  || row >= size || col < 0 || col >= size) {
                printf("Invalid position! Must be 0, 1, or 2.\n");
            } else if (game->board[row][col] != ' ') {
                printf("Cell already taken. Try again.\n");
            } else {
                valid = 1;
            }
        }

        game->board[row][col] = symbol;
        game->move_count++;

        if (check_win(game->board, symbol)) {
            game->game_over = 1;
            game->winner = symbol;
        } else if (game->move_count == 9) {
            game->game_over = 1;
            game->winner = '-';
        } else {
            game->turn = (my_turn == 1) ? 2 : 1;
        }
        pthread_cond_broadcast(&game->cond);
        pthread_mutex_unlock(&game->mutex);
    }

    pthread_mutex_lock(&game->mutex);
    draw_board(game->board);
    printf("============ Game Over ============\n");
    if (game->winner == '-') {
        printf("The game is a draw.\n");
    } else {
        printf("🎉 Player %c wins the game! 🎉\n", game->winner);
    }
    printf("===================================\n");
    pthread_mutex_unlock(&game->mutex);

    shmdt(game);
    if (is_player1) {
        int shm_id = shmget(SHM_KEY, sizeof(SharedGame), 0666);
        if (shm_id != -1) {
            shmctl(shm_id, IPC_RMID, NULL);
        }
    }

    return 0;
}

SharedGame* create_and_map_shared_memory(int is_creator) {
    int shm_id = shmget(SHM_KEY, sizeof(SharedGame), IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("shmget");
        return NULL;
    }

    SharedGame* game = (SharedGame*) shmat(shm_id, NULL, 0);
    if (game == (SharedGame*) -1) {
        perror("shmat");
        return NULL;
    }

    if (is_creator) {
        memset(game, 0, sizeof(SharedGame));
        for (int i = 0; i < size; i++)
            for (int j = 0; j < size; j++)
                game->board[i][j] = ' ';

        game->turn = 1;
        game->move_count = 0;
        game->game_over = 0;
        game->winner = '-';

        pthread_mutexattr_t mattr;
        pthread_condattr_t cattr;

        pthread_mutexattr_init(&mattr);
        pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED);
        pthread_mutex_init(&game->mutex, &mattr);
        pthread_mutexattr_destroy(&mattr);

        pthread_condattr_init(&cattr);
        pthread_condattr_setpshared(&cattr, PTHREAD_PROCESS_SHARED);
        pthread_cond_init(&game->cond, &cattr);
        pthread_condattr_destroy(&cattr);
    }

    return game;
}

void draw_board(char board[size][size]) {
    printf("\n");
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            printf(" %c ", board[i][j]);
            if (j < size - 1) printf("|");
        }
        printf("\n");
        if (i < size - 1) printf("---+---+---\n");
    }
    printf("\n");
}

int check_win(char board[size][size], char sym) {
    for (int i = 0; i < size; i++) {
        if (board[i][0] == sym && board[i][1] == sym && board[i][2] == sym) return 1;
        if (board[0][i] == sym && board[1][i] == sym && board[2][i] == sym) return 1;
    }
    if (board[0][0] == sym && board[1][1] == sym && board[2][2] == sym) return 1;
    if (board[0][2] == sym && board[1][1] == sym && board[2][0] == sym) return 1;
    return 0;
}