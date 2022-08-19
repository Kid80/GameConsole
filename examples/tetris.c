#include "LCD_Driver.h"
#include "LCD_Touch.h"
#include "LCD_GUI.h"
#include "LCD_Bmp.h"
#include "DEV_Config.h"
#include "pico/stdio.h"
#include "hardware/watchdog.h"
#include "pico/stdlib.h"
#include "stdbool.h"
#include "input.h"
#include "tetris.h"
#include "random.h"

/* To Do
Switch down press function to down instead of left
Remove TET_MENU from left press function
*/

enum State{TET_GAME, TET_MENU, TET_GAMEOVER, TET_EXITED};
enum State state;

// Tetris Colours: White, Blue, Cyan, Yellow, Purple, Green, Red, Orange
uint16_t colours[8] = {0xffff, 0x001f, 0x07ff, 0xffe0, 0x780f, 0x07e0, 0xf800, 0xfbe0};

int pieces[4][7][4][2] = {{ {{-1,  0}, { 0,  0}, { 0,  1}, { 1,  0}},   // 0 T
                            {{ 0,  0}, { 0,  1}, { 1,  0}, { 1,  1}},   // 1 Square
                            {{-2,  0}, {-1,  0}, { 0,  0}, { 1,  0}},   // 2 Line
                            {{-1,  0}, { 0,  0}, { 0,  1}, { 1,  1}},   // 3 Right Z
                            {{-1,  1}, { 0,  0}, { 0,  1}, { 1,  0}},   // 4 Left Z
                            {{-1,  0}, { 0,  0}, { 1,  0}, { 1,  1}},   // 5 Right L
                            {{-1,  0}, {-1,  1}, { 0,  0}, { 1,  0}}},  // 6 Left L
                            
                          { {{ 0, -1}, { 0,  0}, { 0,  1}, { 1,  0}},   // 0 T
                            {{ 0,  0}, { 0,  1}, { 1,  0}, { 1,  1}},   // 1 Square
                            {{-1, -1}, {-1,  0}, {-1,  1}, {-1,  2}},   // 2 Line
                            {{ 0,  0}, { 0,  1}, { 1, -1}, { 1,  0}},   // 3 Right Z
                            {{ 0, -1}, { 0,  0}, { 1,  0}, { 1,  1}},   // 4 Left Z
                            {{ 0, -1}, { 0,  0}, { 0,  1}, { 1, -1}},   // 5 Right L
                            {{ 0, -1}, { 0,  0}, { 0,  1}, { 1,  1}}},  // 6 Left L

                          { {{-1,  0}, { 0, -1}, { 0,  0}, { 1,  0}},   // 0 T
                            {{ 0,  0}, { 0,  1}, { 1,  0}, { 1,  1}},   // 1 Square
                            {{-2,  1}, {-1,  1}, { 0,  1}, { 1,  1}},   // 2 Line
                            {{-1, -1}, { 0, -1}, { 0,  0}, { 1,  0}},   // 3 Right Z
                            {{-1,  0}, { 0, -1}, { 0,  0}, { 1, -1}},   // 4 Left Z
                            {{-1, -1}, {-1,  0}, { 0,  0}, { 1,  0}},   // 5 Right L
                            {{-1,  0}, { 0,  0}, { 1, -1}, { 1,  0}}},  // 6 Left L

                          { {{-1,  0}, { 0, -1}, { 0,  0}, { 0,  1}},   // 0 T
                            {{ 0,  0}, { 0,  1}, { 1,  0}, { 1,  1}},   // 1 Square
                            {{ 0, -1}, { 0,  0}, { 0,  1}, { 0,  2}},   // 2 Line
                            {{-1,  0}, {-1,  1}, { 0, -1}, { 0,  0}},   // 3 Right Z
                            {{-1, -1}, {-1,  0}, { 0,  0}, { 0,  1}},   // 4 Left Z
                            {{-1,  1}, { 0, -1}, { 0,  0}, { 0,  1}},   // 5 Right L
                            {{-1, -1}, { 0, -1}, { 0,  0}, { 0,  1}}}}; // 6 Left L

int board[TET_BOARD_X][TET_BOARD_Y];
int lastboard[TET_BOARD_X][TET_BOARD_Y];

piece Piece;
piece Ghost;
piece Ghost2;

// Says whether it is outputting at the moment
bool tet_Outputting;

// Says whether a rerender is required
bool tet_ReRender;

// Says if the IRQ should be ignored
bool tet_IgnoreIRQ;

// Drop speed in cells per second
int G = 0;

// Return codes
int Code = 0;

int Score = 0;

volatile bool isReady;

void tet_showScore(){
    GUI_DisString_EN(5, 5, "Score: ", &Font16, colours[0], 0x0000);
    GUI_DisNum(85, 5, Score, &Font16, colours[0], 0x0000);
}

void tet_addScore(int no){
    Score += no;
    GUI_DrawRectangle(85, 5, 160, 21, colours[0], DRAW_FULL, DOT_PIXEL_DFT);
    GUI_DisNum(85, 5, Score, &Font16, colours[0], 0x0000);
}

// Input handler
void tet_down(){
    switch(state){
        case TET_GAME:
            if(tet_IgnoreIRQ){ break; }
            if(!Piece.isActive){ break; }
            tet_IgnoreIRQ = true;
            if(tet_can_lower()){
                Piece.y--;
                if(!tet_Outputting){
                    tet_IgnoreIRQ = false;
                    tet_output();
                }
                else{
                    tet_ReRender = true;
                }
            }
            tet_IgnoreIRQ = false;
            break;
        case TET_MENU:
            isReady = true;
            break;
        case TET_GAMEOVER:
            break;
        case TET_EXITED:
            break;
    }
}
// Input handler
void tet_left(){
    switch(state){
        case TET_GAME:
            if(tet_IgnoreIRQ){ break; }
            if(!Piece.isActive){ break; }
            tet_IgnoreIRQ = true;
            if(tet_can_left()){
                Piece.x--;
                if(!tet_Outputting){
                    tet_IgnoreIRQ = false;
                    tet_output();
                }
                else{
                    tet_ReRender = true;
                }
            }
            tet_IgnoreIRQ = false;
            break;
        case TET_MENU:
            break;
        case TET_GAMEOVER:
            break;
        case TET_EXITED:
            break;
    }
}
// Input handler
void tet_right(){
    switch(state){
        case TET_GAME:
            if(tet_IgnoreIRQ){ break; }
            if(!Piece.isActive){ break; }
            tet_IgnoreIRQ = true;
            if(tet_can_right()){
                Piece.x++;
                if(!tet_Outputting){
                    tet_IgnoreIRQ = false;
                    tet_output();
                }
                else{
                    tet_ReRender = true;
                }
            }
            tet_IgnoreIRQ = false;
            break;
        case TET_MENU:
            break;
        case TET_GAMEOVER:
            break;
        case TET_EXITED:
            break;
    }
}
// Input handler
void tet_rotate(){
    switch(state){
        case TET_GAME:
            if(tet_IgnoreIRQ){ break; }
            if(!Piece.isActive){ break; }
            tet_IgnoreIRQ = true;
            if(tet_can_rotate()){
                Piece.r = (Piece.r + 1) % 4;
                if(!tet_Outputting){
                    tet_IgnoreIRQ = false;
                    tet_output();
                }
                else{
                    tet_ReRender = true;
                }
            }
            tet_IgnoreIRQ = false;
            break;
        case TET_MENU:
            break;
        case TET_GAMEOVER:
            break;
        case TET_EXITED:
            break;
    }
}

// Checks if the piece can be lowered
bool tet_can_lower(){
    Ghost = Piece;
    Ghost.y--;
    int blockX;
    int blockY;
    for(int i = 0; i < 4; i++){
        blockX = Ghost.x + pieces[Ghost.r][Ghost.i][i][0];
        blockY = Ghost.y + pieces[Ghost.r][Ghost.i][i][1];
        if(blockX < 0 || blockX >= TET_BOARD_X || blockY < 0 || blockY >= TET_BOARD_Y){ return false; }
        if(board[blockX][blockY] != 0){
            return false;
        }
    }
    return true;
}
// Checks if the piece can be rotated
bool tet_can_rotate(){
    Ghost = Piece;
    Ghost.r = (Ghost.r + 1) % 4;
    int blockX;
    int blockY;
    for(int i = 0; i < 4; i++){
        blockX = Ghost.x + pieces[Ghost.r][Ghost.i][i][0];
        blockY = Ghost.y + pieces[Ghost.r][Ghost.i][i][1];
        if(blockX < 0 || blockX >= TET_BOARD_X || blockY < 0 || blockY >= TET_BOARD_Y){ return false; }
        if(board[blockX][blockY] != 0){ return false; }
    }
    return true;
}
// Checks if the piece can be moved left
bool tet_can_left(){
    Ghost = Piece;
    Ghost.x--;
    int blockX;
    int blockY;
    for(int i = 0; i < 4; i++){
        blockX = Ghost.x + pieces[Ghost.r][Ghost.i][i][0];
        blockY = Ghost.y + pieces[Ghost.r][Ghost.i][i][1];
        if(blockX < 0 || blockX >= TET_BOARD_X || blockY < 0 || blockY >= TET_BOARD_Y){ return false; }
        if(board[blockX][blockY] != 0){ return false; }
    }
    return true;
}
// Checks if the piece can be moved right
bool tet_can_right(){
    Ghost = Piece;
    Ghost.x++;
    int blockX;
    int blockY;
    for(int i = 0; i < 4; i++){
        blockX = Ghost.x + pieces[Ghost.r][Ghost.i][i][0];
        blockY = Ghost.y + pieces[Ghost.r][Ghost.i][i][1];
        if(blockX < 0 || blockX >= TET_BOARD_X || blockY < 0 || blockY >= TET_BOARD_Y){ return false; }
        if(board[blockX][blockY] != 0){ return false; }
    }
    return true;
}

/*
Outputs the board and the piece to the screen to the screen.
*/
int tet_output(){
    // Creates the boarder
    GUI_DrawRectangle(160, 5, 320, 315, 0x0000, DRAW_EMPTY, DOT_PIXEL_DFT);

    Ghost2 = Piece;
    tet_Outputting = true;
    tet_ReRender = false;
    tet_IgnoreIRQ = false;
    // Adds the current piece to the board
    int blockX;
    int blockY;
    for(int i = 0; i < 4; i++){
        blockX = Ghost2.x + pieces[Ghost2.r][Ghost2.i][i][0];
        blockY = Ghost2.y + pieces[Ghost2.r][Ghost2.i][i][1];
        if(Ghost2.isActive){
            if(blockY >= 0 && blockX >= 0 && blockX < TET_BOARD_X && blockY < TET_BOARD_Y){
                board[blockX][blockY] = Ghost2.c;
            }
            else{ return 1; }
        }
    }

    // Updates the board
    uint16_t xpos = 0;
    int ypos = 0;
    for(int y = 0; y < 20; y++){
        for(int x = 0; x < TET_BOARD_X; x++){
            xpos = x * 15 + 165;
            ypos = y * -15 + 295;
            uint16_t Ypos = ypos;
            // If statement removed to fix ghosting issue
            if(board[x][y] != lastboard[x][y]){
                GUI_DrawRectangle(xpos, Ypos, xpos + 14, Ypos + 14, colours[board[x][y]], DRAW_FULL, DOT_PIXEL_DFT);
            }
        }
    }

    // Sets lastboard to the current board
    for(int y = 0; y < TET_BOARD_Y; y++){
        for(int x = 0; x < TET_BOARD_X; x++){
            lastboard[x][y] = board[x][y];
        }
    }

    // Removes the current piece from the board
    for(int i = 0; i < 4; i++){
        blockY = Ghost2.y + pieces[Ghost2.r][Ghost2.i][i][1];
        if(Ghost2.isActive){
            blockX = Ghost2.x + pieces[Ghost2.r][Ghost2.i][i][0];
            if(blockY >= 0 && blockX >= 0 && blockX < TET_BOARD_X && blockY < TET_BOARD_Y){
                board[blockX][blockY] = 0;
            }
            else{
                return 2;
            }
        }
    }

    if(tet_ReRender){
        tet_ReRender = false;
        tet_output();
    }
    tet_Outputting = false;
    return 0;
}

/*
Starts the tetris game

Return Codes:
0 - Game exited successfully
1 - Piece position error
2 - Piece out of bounds when being cleared from the board
*/
int tetris(){
    tet_Outputting = false;
    tet_ReRender = false;
    state = TET_MENU;

    // Note: d_down should be changed to tet_drop when gpio 2 is working
    input_init();
    input_set_d_up(&tet_rotate);
    input_set_d_down(&tet_down);
    input_set_d_left(&tet_left);
    input_set_d_right(&tet_right);

    Code = tet_mainloop();
    return Code;
}

void tet_end(){
    input_clear();
}

int tet_mainloop(){
    while(1){
        switch(state){
            case TET_GAME:
                Code = tet_gameloop();
                if(Code != 0){ return Code; }
                break;
            case TET_MENU:
                Code = tet_menu();
                if(Code != 0){ return Code; }
                break;
            case TET_GAMEOVER:
                Code = tet_gameover();
                break;
            case TET_EXITED:
                tet_end();
                return 0;
                break;
        }
    }
}

int tet_menu(){
    bool started = false;
    GUI_Clear(colours[1]);
    GUI_DisChar(189, 5, 'T', &Font24, colours[1], colours[6]);
    GUI_DisChar(206, 5, 'E', &Font24, colours[1], colours[7]);
    GUI_DisChar(223, 5, 'T', &Font24, colours[1], colours[3]);
    GUI_DisChar(240, 5, 'R', &Font24, colours[1], colours[5]);
    GUI_DisChar(257, 5, 'I', &Font24, colours[1], colours[2]);
    GUI_DisChar(274, 5, 'S', &Font24, colours[1], colours[4]);

    GUI_DisString_EN(78, 55, "Press down to play!", &Font24, colours[1], 0x0000);

    isReady = false;
    while(1){
        if(isReady == true){
            break;
        }
    }
    GUI_Clear(colours[0]);
    state = TET_GAME;
    return 0;
}

int tet_gameover(){
    return 0;
}

int tet_gameloop(){
    Piece.isActive = false;
    bool isRunning = true;
    while(isRunning){
        Driver_Delay_ms(1000);
        if(tet_can_lower()){
            Piece.y--;
        }
        else{
            Piece.isActive = false;
            int blockX;
            int blockY;
            for(int i = 0; i < 4; i++){
                int blockX = Piece.x + pieces[Piece.r][Piece.i][i][0];
                int blockY = Piece.y + pieces[Piece.r][Piece.i][i][1];
                if(blockY >= 20){
                    state = TET_GAMEOVER;
                    return 0;
                }
                board[blockX][blockY] = Piece.c;
            }
            tet_checkBoard();
        }
        if(!Piece.isActive){
            tet_GeneratePiece();
        }
        Code = tet_output();
        if(Code != 0){
            return Code + 10;
        }
    }
    return 0;
}

void tet_GeneratePiece(){
    Piece.x = 4;
    Piece.y = 20;
    Piece.c = random_byte() & 0x07;
    while(Piece.c == 0){
        Piece.c = random_byte() & 0x07;
    }
    Piece.i = random_byte() & 0x07;
    while(Piece.i == 7){
        Piece.i = random_byte() & 0x07;
    }
    Piece.r = 0;
    Piece.isActive = true;
}

void tet_checkBoard(){
    bool isLine;
    int noLines = 0;
    for(int y = 0; y < TET_BOARD_Y; y++){
        isLine = true;
        for(int x = 0; x < TET_BOARD_X; x++){
            if(board[x][y] == 0){
                isLine = false;
            }
        }
        if(isLine){
            for(int x = 0; x < TET_BOARD_X; x++){
                board[x][y] = 0;
            }
            for(int y1 = y + 1; y1 < TET_BOARD_Y; y1++){
                for(int x = 0; x < TET_BOARD_X; x++){
                    board[x][y1 - 1] = board[x][y1];
                }
            }
            noLines++;
        }
    }
    switch (noLines)
    {
        case 0:
            break;
        case 1:
            tet_addScore(100);
            break;
        case 2:
            tet_addScore(300);
            break;
        case 3:
            tet_addScore(500);
            break;
        case 4:
            tet_addScore(800);
            break;
        default:
            break;
    }
}