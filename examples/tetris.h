#ifndef __TETRIS_H
#define __TETRIS_H

#define TET_BOARD_X 10
#define TET_BOARD_Y 24

/*
r = the rotation
i = the piece number
c = colour
*/
struct piece{
    int x;
    int y;
    int r;
    int i;
    int c;
    bool isActive;
} typedef piece;

// Input handlers
void tet_down();
void tet_left();
void tet_right();
void tet_rotate();
// Functions for checking if the piece can do an operation
bool tet_can_lower();
bool tet_can_rotate();
bool tet_can_left();
bool tet_can_right();

int tet_output();
int tetris();
void tet_end();

int tet_mainloop();

int tet_menu();
int tet_gameloop();
int tet_gameover();
void tet_GeneratePiece();
void tet_checkBoard();

#endif