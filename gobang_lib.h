#include <windows.h>
#include <gl/gl.h>
#include <math.h>
#include <stdarg.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>                          // to use console to output test inf, can be deleted later
#include <io.h>                             // to use console to output test inf, can be deleted later
#include <fcntl.h>                          // to use console to output test inf, can be deleted later

#define MAIN_WINDOW_HEIGHT 629              // client area`s length is 600px, two edges of the window are 29px high in total
#define MAIN_WINDOW_WIDTH 607               // client area`s width is 600px, two edges of the window are 7px wide in total
#define CLIENT_AREA_HEIGHT 600
#define CLIENT_AREA_WIDTH 600
#define CHESSBOARD_R 0.75f
#define CHESSBOARD_G 0.45f
#define CHESSBOARD_B 0.15f
//#define CHESSBOARD_R 0.98f
//#define CHESSBOARD_G 0.71f
//#define CHESSBOARD_B 0.33f
#define CHESSBOARD_LINE_NUMBER 15
#define CHESSBOARD_ROW_START -0.7f
#define CHESSBOARD_ROW_END 0.7f
#define CHESSBOARD_ROW_STEP 0.1f
#define CHESSBOARD_COLUMN_START -0.8f
#define CHESSBOARD_COLUMN_END 0.6f
#define CHESSBOARD_COLUMN_STEP 0.1f
#define CHESS_RADIUS 15
#define WHITE_CHESS 1
#define BLACK_CHESS -1
#define TIE 0
#define REAL_CHESS 1
#define BRAIN_CHESS 0
#define MAX_LONG 2000000000
#define MIN_LONG -2000000000
#define FLASH_FRAME_NUMBER 500
#define MAX_CHESS_RAIN_NUMBER 6
#define MAX_CHESS_RAIN_LENGTH 7
#define MIN_CHESS_RAIN_LENGTH 3
#define MAX_CHESS_RAIN_SPEED 3
#define MIN_CHESS_RAIN_SPEED 1
#define CHESS_RAIN_SPEED_INDEX 500
#define CHESS_RAIN_GENERATING_PROBABILITY_INDEX 10
#define CHESS_RAIN_GENERATING_PROBABILITY 0.7
#define MAX_CHESS_RAIN_X_POSITION 19
#define MIN_CHESS_RAIN_X_POSITION -19
#define CHESS_RAIN_X_POSITION_INDEX 20

#define MENU_START 1
#define MENU_RULES 2
#define MENU_SETTINGS 3
#define MENU_TEAM 4
#define MENU_QUIT 5
#define SUB_MENU_HUMAN 6
#define SUB_MENU_COMPUTER 7
#define DIFFICULT_EASY 8
#define DIFFICULT_NORMAL 9
#define DIFFICULT_HARD 10
#define DIFFICULT_HELL 11
#define GAME_UNDO 12
#define GAME_RESTART 13
#define SUB_MENU_RETURN 14
#define DIFFICULT_RETURN 15
#define SIDE_BLACK 16
#define SIDE_WHITE 17
#define SIDE_RETURN 18
#define GAME_RETURN 19
#define SETTINGS_CHESS_RAIN 20

#define MAX(arg_a, arg_b) ((arg_a) > (arg_b) ? (arg_a) : (arg_b))

#define MIN(arg_a, arg_b) ((arg_a) < (arg_b) ? (arg_a) : (arg_b))

#define CHECK_WITHIN_CHESS_RADIUS(arg_x, arg_y) (pow((arg_x)-index_to_px_x(CHESSBOARD_COLUMN_START+CHESSBOARD_COLUMN_STEP*loop_i), 2) \
                                                +pow((arg_y)-index_to_px_y(CHESSBOARD_ROW_END-CHESSBOARD_ROW_STEP*loop_j), 2) \
                                                <= CHESS_RADIUS*CHESS_RADIUS)
// well, because two coordinates are upside down and what`s more, they are oppsite in x and y.
// so when it comes to rows, it should be "ROW_END-" while COLUMN`s is "COLUMN_START+" in the macro definition

#define MINI_FLASH(arg_x, arg_y) (arg_x)-(float)tick/FLASH_FRAME_NUMBER*3*(0.7-(arg_x)), (arg_y)+(float)tick/FLASH_FRAME_NUMBER*3*(arg_y)

struct chess_node
{
    int x, y;
};

struct rain_node
{
    int length;                 // MIN_CHESS_RAIN_LENGTH ~ MAX_CHESS_RAIN_LENGTH
    int distribution;           // 0 ~ (1<<length)-1
    float speed;                // (MIN_CHESS_RAIN_SPEED ~ MAX_CHESS_RAIN_SPEED)/CHESS_RAIN_SPEED_INDEX
    float position_y;           // 1.05f+length ~ -1.05f
    float position_x;           // (MIN_CHESS_RAIN_X_POSITION ~ MAX_CHESS_RAIN_X_POSITION)/CHESS_RAIN_X_POSITION_INDEX
};

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);

void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);

void DisableOpenGL(HWND, HDC, HGLRC);

void chess_judgement(int, int, int, int, int);

void InitConsoleWindow();                   // to use console to output test inf, can be deleted later

int check_win(int, int);

double px_to_index_x(int);

double px_to_index_y(int);

int index_to_px_x(double);

int index_to_px_y(double);

void draw_solid_circle(float, float, float, int);

void draw_wired_circle(float, float, float, int);

void chessable_judgement(int, int);

void restart();

void undo_judgement(int, int, int, int);

void restart_judgement(int, int, int, int);

void draw_interface();

void brain_chess_undo();

void brain_chess_deactivate();

long evaluation(int);

int evaluate_check(int, int, int, int, ...);

void computer_chess(int, int, float);

int search_check(int, int);

long computer_search(int, float, int, long, long);

void draw_cn_string(const char *);

void selectFont(int, int, const char *);

int check_button(int, int);

void draw_menu();

void menu_button_down_judgement(int, int, int, int);

void return_to_menu_judgement(int, int, int, int);

void draw_mini_chessboard(long);

void draw_chess_rain();

int search_count();

void draw_ratio(int, float, float);
