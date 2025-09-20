#include <curses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>


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
    ts.tv_nsec = 1;   // nanoseconds (0.5s)
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

void update_cell(Board *game, int r, int c, int value) {
    game->cells[r][c] = value;
    mvprintw(r, c *2, "%d", value); // c * 2 leaves spacing between for cleaner visuals

    struct timespec ts;
    ts.tv_sec = 0;            // seconds
    ts.tv_nsec = 1;   // nanoseconds (0.5s)
    nanosleep(&ts, NULL);
    refresh();
}

void shuffle(int *array, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);  // random index in [0, i]
        // swap array[i] and array[j]
        int temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

void visit(int x, int y, Board *game) {
    // randomly selecting the order of the 4 cells for maze traversial
    // base case check inside the grid
    if (x < 0 || x >= game->rows || y < 0 || y >= game->cols) {
        return;
    }
    // base case check if the cell is already visited
    if (game->cells[x][y] == 1) {
        return;
    }

    // marking the cell
    update_cell(game, x, y, 1);


    int r = rand() % 4;
    int dirs[4] = {0, 1, 2, 3};
    shuffle(dirs, 4);

    for (int i = 0; i < 4; i++) {
        switch (dirs[i]) {
            case 0: // UP
                // far room two up: (x, y-2); wall one up: (x, y-1)
                if (x - 2 >= 0 && game->cells[x - 2][y] == 0) {
                    update_cell(game, x - 1, y, 1);  // knock down wall
                    visit(x - 2, y, game);           // recurse into far room
                }
                break;

            case 1: // RIGHT
                // far room two right: (x+2, y); wall one right: (x+1, y)
                if (y + 2 < game->cols && game->cells[x][y + 2] == 0) {
                    update_cell(game, x, y + 1, 1);
                    visit(x, y + 2, game);
                }
                break;

            case 2: // DOWN
                // far room two down: (x, y+2); wall one down: (x, y+1)
                if (x + 2 < game->rows && game->cells[x + 2][y] == 0) {
                    update_cell(game, x + 1, y, 1);
                    visit(x + 2, y, game);
                }
                break;

            case 3: // LEFT
                // far room two left: (x-2, y); wall one left: (x-1, y)
                if (y - 2 >= 0 && game->cells[x][y -2] == 0) {
                    update_cell(game, x, y - 1, 1);
                    visit(x, y - 2, game);
                }
                break;
        }
    }

}


int main() {
        srand((unsigned)time(NULL));
    // allocate memory
    Board game = allocate_2d_array(15,15);

    // seeding
    srand((unsigned)time(NULL));

    // Initializing curses
    initscr();

    // start of maze generation
    full_update_board_1by1(&game,0);
    visit(4, 3, &game);

    print_board(&game);

    // configure the terminal
    noecho(); // prevents echoing keys to the screen
    cbreak(); // line buffering disabled
    keypad(stdscr, TRUE); // enable special keys.

    // start of main logic
    //addstr("Hello, World!");
    refresh();

    //full_update_board_1by1(&game, 1);
        
    getch();
    endwin(); // cleaning up before closing.
    return 0;
}
