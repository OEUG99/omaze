#include <curses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

// Box drawing characters for clean maze walls
#define WALL_H        '─'  // Horizontal line
#define WALL_V        '│'  // Vertical line
#define WALL_TL       '┌'  // Top-left corner
#define WALL_TR       '┐'  // Top-right corner
#define WALL_BL       '└'  // Bottom-left corner
#define WALL_BR       '┘'  // Bottom-right corner
#define WALL_CROSS    '┼'  // Cross/intersection
#define WALL_T_UP     '┴'  // T pointing upp
#define WALL_T_DOWN   '┬'  // T pointing down
#define WALL_T_LEFT   '┤'  // T pointing leftt
#define WALL_T_RIGHT  '├'  // T pointing right

typedef struct {
    int rows;
    int cols;
    int **cells;   // 2D array for the board state
} Board;

Board allocate_2d_array(int rows, int cols){

    int **arr = malloc(rows * sizeof *arr);  // allocate array of int* (row pointers)
    
    for (int r = 0; r < rows; r++) {
        arr[r] = malloc(cols * sizeof *arr[r]);  // allocate each row
    
        for (int c = 0; c < cols; c++) {
            arr[r][c] = 0;   // <-- assign a default value here
        }


    }

    Board game = {rows,cols,arr};

    return game;

}


void print_board(const Board *b) {
    for (int r = 0; r < b->rows; r++) {
        for (int c = 0; c < b->cols; c++) {
            mvprintw(r, c * 2, "%d", b->cells[r][c]); // c * 2 leaves spacing between for cleaner visuals
        }
    }
}

void full_update_board_1by1(Board *game, int value) {

    struct timespec ts;
    ts.tv_sec = 0;            // seconds
    ts.tv_nsec = 100000000;   // nanoseconds (0.5s)
    curs_set(0);

    for (int r = 0; r < game->rows; r++) {
        for (int c = 0; c < game->cols; c++) {
            if (game->cells[r][c] == 0) {
                game->cells[r][c] = value;
                mvprintw(r, c *2, "%d", value); // c * 2 leaves spacing between for cleaner visuals
                nanosleep(&ts, NULL);
                refresh();
            }
        }
    }

    curs_set(1);

}


int main() {
    // allocate memory
    Board game = allocate_2d_array(10,10);

    // Initializing curses
    initscr();


    print_board(&game);

    // configure the terminal
    noecho(); // prevents echoing keys to the screen
    cbreak(); // line buffering disabled
    keypad(stdscr, TRUE); // enable special keys.

    // start of main logic
    //addstr("Hello, World!");
    refresh();

    full_update_board_1by1(&game, 1);
        
    getch();
    endwin(); // cleaning up before closing.
    return 0;
}
