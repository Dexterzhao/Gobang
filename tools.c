#include "gobang_lib.h"

extern int chessboard[CHESSBOARD_LINE_NUMBER][CHESSBOARD_LINE_NUMBER];
extern int win_sign_start_x, win_sign_start_y, win_sign_end_x, win_sign_end_y, button_down;
extern int chess_sign, chess_sign_x, chess_sign_y, game_abort, win_side, turn_number, brain_turn_number;
extern struct chess_node chess_order[CHESSBOARD_LINE_NUMBER*CHESSBOARD_LINE_NUMBER+1];
extern int menu_choice, sub_menu_choice, difficult_choice, side_choice, human_side, computer_depth, flash_done;
extern float computer_defense;

int chess_rain_enable;

double px_to_index_x(int px)
{
    return (px/CLIENT_AREA_HEIGHT-0.5f)*2.0f;
}

double px_to_index_y(int px)
{
    return (px/CLIENT_AREA_WIDTH-0.5f)*2.0f;
}

int index_to_px_x(double index)
{
    return (int)((index/2+0.5f)*CLIENT_AREA_HEIGHT);
}

int index_to_px_y(double index)
{
    return (int)((index/2+0.5f)*CLIENT_AREA_WIDTH);
}

void draw_solid_circle(float x, float y, float radius, int sections)
{
    int count;
    GLfloat TWOPI=2.0f*3.14159f;

    glBegin(GL_TRIANGLE_FAN);
    {
        glVertex2f(x, y);
        for(count = 0; count <= sections; count++)
        {
            glVertex2f(x+radius*cos(count*TWOPI/sections), y+radius*sin(count*TWOPI/sections));
        }
    }
    glEnd();
}

void draw_wired_circle(float x, float y, float radius, int sections)
{
    int count;
    GLfloat TWOPI = 2.0f*3.14159f;

    glBegin(GL_LINE_STRIP);
    {
        for(count = 0; count <= sections; count++)
        {
            glVertex2f(x+radius*cos(count*TWOPI/sections), y+radius*sin(count*TWOPI/sections));
        }
    }
    glEnd();
}

void draw_interface()
{
    glLineWidth(1.0);           // to draw chessboard lines
    glBegin(GL_LINES);
    {
        for (int loop = 0; loop < CHESSBOARD_LINE_NUMBER; ++loop)
        {
            glColor3f(0.0f, 0.0f, 0.0f);
            glVertex2f(CHESSBOARD_COLUMN_START, CHESSBOARD_ROW_START+CHESSBOARD_ROW_STEP*loop);
            glVertex2f(CHESSBOARD_COLUMN_END, CHESSBOARD_ROW_START+CHESSBOARD_ROW_STEP*loop);
            glColor3f(0.0f, 0.0f, 0.0f);
            glVertex2f(CHESSBOARD_COLUMN_START+CHESSBOARD_COLUMN_STEP*loop, CHESSBOARD_ROW_START);
            glVertex2f(CHESSBOARD_COLUMN_START+CHESSBOARD_COLUMN_STEP*loop, CHESSBOARD_ROW_END);
        }
    }
    glEnd();

    glColor3f(0.0f, 0.0f, 0.0f);             // to draw chessboard points
    draw_solid_circle(-0.1f, 0.0f, 0.012f, 10);
    draw_solid_circle(-0.1f, 0.4f, 0.012f, 10);
    draw_solid_circle(-0.1f, -0.4f, 0.012f, 10);
    draw_solid_circle(-0.5f, 0.0f, 0.012f, 10);
    draw_solid_circle(0.3f, 0.0f, 0.012f, 10);
    draw_solid_circle(-0.5f, 0.4f, 0.012f, 10);
    draw_solid_circle(0.3f, 0.4f, 0.012f, 10);
    draw_solid_circle(-0.5f, -0.4f, 0.012f, 10);
    draw_solid_circle(0.3f, -0.4f, 0.012f, 10);

//    glPointSize(25);            // to draw black and white chess
//    glBegin(GL_POINTS);
//    {
//        for (int loop = 1; loop <= brain_turn_number; ++loop)
//        {
//            if (chessboard[chess_order[loop].x][chess_order[loop].y] > 0)
//            {
//                glColor3f(1.0f, 1.0f, 1.0f);
//                glVertex2f(CHESSBOARD_COLUMN_START+CHESSBOARD_COLUMN_STEP*chess_order[loop].x, CHESSBOARD_ROW_START+CHESSBOARD_ROW_STEP*chess_order[loop].y);
//            }
//            else if (chessboard[chess_order[loop].x][chess_order[loop].y] < 0)
//            {
//                glColor3f(0.0f, 0.0f, 0.0f);
//                glVertex2f(CHESSBOARD_COLUMN_START+CHESSBOARD_COLUMN_STEP*chess_order[loop].x, CHESSBOARD_ROW_START+CHESSBOARD_ROW_STEP*chess_order[loop].y);
//            }
//        }
//    }
//    glEnd();

    for (int loop = 1; loop <= brain_turn_number; ++loop)       // to draw black and white chess
    {
        if (chessboard[chess_order[loop].x][chess_order[loop].y] > 0)
        {
            glColor3f(1.0f, 1.0f, 1.0f);
            draw_solid_circle(CHESSBOARD_COLUMN_START+CHESSBOARD_COLUMN_STEP*chess_order[loop].x, CHESSBOARD_ROW_START+CHESSBOARD_ROW_STEP*chess_order[loop].y, 0.05f, 100);
        }
        else if (chessboard[chess_order[loop].x][chess_order[loop].y] < 0)
        {
            glColor3f(0.0f, 0.0f, 0.0f);
            draw_solid_circle(CHESSBOARD_COLUMN_START+CHESSBOARD_COLUMN_STEP*chess_order[loop].x, CHESSBOARD_ROW_START+CHESSBOARD_ROW_STEP*chess_order[loop].y, 0.05f, 100);
        }
    }

    glLineWidth(4.0);
    glBegin(GL_LINES);
    {
        if (game_abort && win_side)         // to draw a line marking out the winning five chess
        {
            glColor3f(0.0f, 1.0f, 1.0f);
            glVertex2f(CHESSBOARD_COLUMN_START+CHESSBOARD_COLUMN_STEP*win_sign_start_x, CHESSBOARD_ROW_START+CHESSBOARD_ROW_STEP*win_sign_start_y);
            glVertex2f(CHESSBOARD_COLUMN_START+CHESSBOARD_COLUMN_STEP*win_sign_end_x, CHESSBOARD_ROW_START+CHESSBOARD_ROW_STEP*win_sign_end_y);
        }

        for (int loop = turn_number+1; loop <= brain_turn_number; ++loop)           // to mark out those brain chess
        {
            glColor3f(0.0f, 0.0f, 1.0f);
            glVertex2f(CHESSBOARD_COLUMN_START+CHESSBOARD_COLUMN_STEP*(chess_order[loop].x-0.55f), CHESSBOARD_ROW_START+CHESSBOARD_ROW_STEP*(chess_order[loop].y-0.5f));
            glVertex2f(CHESSBOARD_COLUMN_START+CHESSBOARD_COLUMN_STEP*(chess_order[loop].x+0.55f), CHESSBOARD_ROW_START+CHESSBOARD_ROW_STEP*(chess_order[loop].y-0.5f));
            glColor3f(0.0f, 0.0f, 1.0f);
            glVertex2f(CHESSBOARD_COLUMN_START+CHESSBOARD_COLUMN_STEP*(chess_order[loop].x-0.5f), CHESSBOARD_ROW_START+CHESSBOARD_ROW_STEP*(chess_order[loop].y-0.55f));
            glVertex2f(CHESSBOARD_COLUMN_START+CHESSBOARD_COLUMN_STEP*(chess_order[loop].x-0.5f), CHESSBOARD_ROW_START+CHESSBOARD_ROW_STEP*(chess_order[loop].y+0.55f));
            glColor3f(0.0f, 0.0f, 1.0f);
            glVertex2f(CHESSBOARD_COLUMN_START+CHESSBOARD_COLUMN_STEP*(chess_order[loop].x+0.5f), CHESSBOARD_ROW_START+CHESSBOARD_ROW_STEP*(chess_order[loop].y+0.55f));
            glVertex2f(CHESSBOARD_COLUMN_START+CHESSBOARD_COLUMN_STEP*(chess_order[loop].x+0.5f), CHESSBOARD_ROW_START+CHESSBOARD_ROW_STEP*(chess_order[loop].y-0.55f));
            glColor3f(0.0f, 0.0f, 1.0f);
            glVertex2f(CHESSBOARD_COLUMN_START+CHESSBOARD_COLUMN_STEP*(chess_order[loop].x+0.55f), CHESSBOARD_ROW_START+CHESSBOARD_ROW_STEP*(chess_order[loop].y+0.5f));
            glVertex2f(CHESSBOARD_COLUMN_START+CHESSBOARD_COLUMN_STEP*(chess_order[loop].x-0.55f), CHESSBOARD_ROW_START+CHESSBOARD_ROW_STEP*(chess_order[loop].y+0.5f));
        }

        if (chess_sign)         // to draw a hint rectangle of current chess position
        {
            glColor3f(0.0f, 1.0f, 0.0f);
            glVertex2f(CHESSBOARD_COLUMN_START+CHESSBOARD_COLUMN_STEP*(chess_sign_x-0.55f), CHESSBOARD_ROW_START+CHESSBOARD_ROW_STEP*(chess_sign_y-0.5f));
            glVertex2f(CHESSBOARD_COLUMN_START+CHESSBOARD_COLUMN_STEP*(chess_sign_x+0.55f), CHESSBOARD_ROW_START+CHESSBOARD_ROW_STEP*(chess_sign_y-0.5f));
            glColor3f(0.0f, 1.0f, 0.0f);
            glVertex2f(CHESSBOARD_COLUMN_START+CHESSBOARD_COLUMN_STEP*(chess_sign_x-0.5f), CHESSBOARD_ROW_START+CHESSBOARD_ROW_STEP*(chess_sign_y-0.55f));
            glVertex2f(CHESSBOARD_COLUMN_START+CHESSBOARD_COLUMN_STEP*(chess_sign_x-0.5f), CHESSBOARD_ROW_START+CHESSBOARD_ROW_STEP*(chess_sign_y+0.55f));
            glColor3f(0.0f, 1.0f, 0.0f);
            glVertex2f(CHESSBOARD_COLUMN_START+CHESSBOARD_COLUMN_STEP*(chess_sign_x+0.5f), CHESSBOARD_ROW_START+CHESSBOARD_ROW_STEP*(chess_sign_y+0.55f));
            glVertex2f(CHESSBOARD_COLUMN_START+CHESSBOARD_COLUMN_STEP*(chess_sign_x+0.5f), CHESSBOARD_ROW_START+CHESSBOARD_ROW_STEP*(chess_sign_y-0.55f));
            glColor3f(0.0f, 1.0f, 0.0f);
            glVertex2f(CHESSBOARD_COLUMN_START+CHESSBOARD_COLUMN_STEP*(chess_sign_x+0.55f), CHESSBOARD_ROW_START+CHESSBOARD_ROW_STEP*(chess_sign_y+0.5f));
            glVertex2f(CHESSBOARD_COLUMN_START+CHESSBOARD_COLUMN_STEP*(chess_sign_x-0.55f), CHESSBOARD_ROW_START+CHESSBOARD_ROW_STEP*(chess_sign_y+0.5f));
        }                                   // slight difference makes up the slight gap in the four corners
    }
    glEnd();

    if (button_down == GAME_UNDO)
    {
        glColor3f(0.0f, 0.0f, 0.0f);
    }
    else
    {
        glColor3f(1.0f, 1.0f, 1.0f);
    }
    glRasterPos2f(0.91f, 0.53f);
    draw_cn_string("悔");
    glRasterPos2f(0.91f, 0.43f);
    draw_cn_string("棋");

    if (button_down == GAME_RESTART)
    {
        glColor3f(0.0f, 0.0f, 0.0f);
    }
    else
    {
        glColor3f(1.0f, 1.0f, 1.0f);
    }
    glRasterPos2f(0.91f, 0.03f);
    draw_cn_string("再");
    glRasterPos2f(0.91f, -0.07f);
    draw_cn_string("来");

    if (button_down == GAME_RETURN)
    {
        glColor3f(0.0f, 0.0f, 0.0f);
    }
    else
    {
        glColor3f(1.0f, 1.0f, 1.0f);
    }
    glRasterPos2f(0.91f, -0.47f);
    draw_cn_string("返");
    glRasterPos2f(0.91f, -0.57f);
    draw_cn_string("回");

//    glPointSize(30.5);                      // there will be a slight gap if change 30.5 into 30
//    glBegin(GL_POINTS);                     // to draw some buttons
//    {
//        glColor3f(0.0f, 0.0f, 0.0f);
//        glVertex2f(0.95f, 0.55f);
//        glColor3f(0.0f, 0.0f, 0.0f);
//        glVertex2f(0.95f, 0.45f);
//
//        glColor3f(1.0f, 1.0f, 1.0f);
//        glVertex2f(0.95f, 0.05f);
//        glColor3f(1.0f, 1.0f, 1.0f);
//        glVertex2f(0.95f, -0.05f);
//    }
//    glEnd();
}

void draw_menu()
{
    static int tick = 0;

    if (chess_rain_enable)
    {
        draw_chess_rain();
    }

    selectFont(70, DEFAULT_CHARSET, "华文仿宋");
    glColor3f(1.0f, 1.0f, 0.0f);
    glRasterPos2f(-0.72f, 0.4f);
    draw_cn_string("五子棋");

    selectFont(30, DEFAULT_CHARSET, "华文仿宋");

    if (menu_choice)
    {
        if (sub_menu_choice)
        {
            if (difficult_choice)
            {
                if (side_choice)
                {
                    if (!flash_done)
                    {
                        ++tick;
                        draw_mini_chessboard(tick);
                        if (tick >= FLASH_FRAME_NUMBER)
                        {
                            flash_done = 1;
                        }
                    }
                    else
                    {
                        // oops, it won`t get here
                    }
                }
                else
                {
                    if (button_down == SIDE_BLACK)
                    {
                        glColor3f(1.0f, 1.0f, 1.0f);
                    }
                    else
                    {
                        glColor3f(1.0f, 1.0f, 0.0f);
                    }
                    glRasterPos2f(-0.58f, 0.22f);
                    draw_cn_string("执黑先行");

                    if (button_down == SIDE_WHITE)
                    {
                        glColor3f(1.0f, 1.0f, 1.0f);
                    }
                    else
                    {
                        glColor3f(1.0f, 1.0f, 0.0f);
                    }
                    glRasterPos2f(-0.58f, 0.02f);
                    draw_cn_string("执白后行");

                    if (button_down == SIDE_RETURN)
                    {
                        glColor3f(1.0f, 1.0f, 1.0f);
                    }
                    else
                    {
                        glColor3f(1.0f, 1.0f, 0.0f);
                    }
                    glRasterPos2f(-0.49f, -0.58f);
                    draw_cn_string("返回");

                    draw_mini_chessboard(tick);
                }
            }
            else
            {
                if (button_down == DIFFICULT_EASY)
                {
                    glColor3f(1.0f, 1.0f, 1.0f);
                }
                else
                {
                    glColor3f(1.0f, 1.0f, 0.0f);
                }
                glRasterPos2f(-0.58f, 0.22f);
                draw_cn_string("初出茅庐");

                if (button_down == DIFFICULT_NORMAL)
                {
                    glColor3f(1.0f, 1.0f, 1.0f);
                }
                else
                {
                    glColor3f(1.0f, 1.0f, 0.0f);
                }
                glRasterPos2f(-0.58f, 0.02f);
                draw_cn_string("小试牛刀");

                if (button_down == DIFFICULT_HARD)
                {
                    glColor3f(1.0f, 1.0f, 1.0f);
                }
                else
                {
                    glColor3f(1.0f, 1.0f, 0.0f);
                }
                glRasterPos2f(-0.58f, -0.18f);
                draw_cn_string("崭露头角");

                if (button_down == DIFFICULT_HELL)
                {
                    glColor3f(1.0f, 1.0f, 1.0f);
                }
                else
                {
                    glColor3f(1.0f, 1.0f, 0.0f);
                }
                glRasterPos2f(-0.58f, -0.38f);
                draw_cn_string("颇有建树");

                if (button_down == DIFFICULT_RETURN)
                {
                    glColor3f(1.0f, 1.0f, 1.0f);
                }
                else
                {
                    glColor3f(1.0f, 1.0f, 0.0f);
                }
                glRasterPos2f(-0.49f, -0.58f);
                draw_cn_string("返回");

                draw_mini_chessboard(tick);
            }
        }
        else if (menu_choice == MENU_START)
        {
            if (button_down == SUB_MENU_HUMAN)
            {
                glColor3f(1.0f, 1.0f, 1.0f);
            }
            else
            {
                glColor3f(1.0f, 1.0f, 0.0f);
            }
            glRasterPos2f(-0.58f, 0.22f);
            draw_cn_string("人人对战");

            if (button_down == SUB_MENU_COMPUTER)
            {
                glColor3f(1.0f, 1.0f, 1.0f);
            }
            else
            {
                glColor3f(1.0f, 1.0f, 0.0f);
            }
            glRasterPos2f(-0.58f, 0.02f);
            draw_cn_string("人机对战");

            if (button_down == SUB_MENU_RETURN)
            {
                glColor3f(1.0f, 1.0f, 1.0f);
            }
            else
            {
                glColor3f(1.0f, 1.0f, 0.0f);
            }
            glRasterPos2f(-0.49f, -0.58f);
            draw_cn_string("返回");

            draw_mini_chessboard(tick);
        }
        else
        {
            if (menu_choice == MENU_RULES)
            {
                glColor3f(1.0f, 1.0f, 1.0f);
                glRasterPos2f(0.2f, 0.22f);
                draw_cn_string("黑方先行");
                glRasterPos2f(0.2f, 0.02f);
                draw_cn_string("五子连珠即胜");
                glRasterPos2f(0.2f, -0.18f);
                draw_cn_string("暂无禁手规则");
                glRasterPos2f(0.2f, -0.38f);
                draw_cn_string("右键辅助思考");
                glRasterPos2f(0.2f, -0.58f);
                draw_cn_string("中键可以清空");
            }
            else if (menu_choice == MENU_SETTINGS)
            {
                glColor3f(1.0f, 1.0f, 1.0f);
                glRasterPos2f(0.2f, 0.22f);
                draw_cn_string("使用棋子雨");

                draw_ratio(chess_rain_enable, 0.7f, 0.245f);
            }
            else if (menu_choice == MENU_TEAM)
            {
                glColor3f(1.0f, 1.0f, 1.0f);
                glRasterPos2f(0.2f, 0.22f);
                draw_cn_string("华中科技大学");
                glRasterPos2f(0.2f, 0.02f);
                draw_cn_string("ACM2012");
                glRasterPos2f(0.2f, -0.18f);
                draw_cn_string("严雨辰, 赵子昂");
            }

            if (button_down == SUB_MENU_RETURN)
            {
                glColor3f(1.0f, 1.0f, 1.0f);
            }
            else
            {
                glColor3f(1.0f, 1.0f, 0.0f);
            }
            glRasterPos2f(-0.49f, -0.58f);
            draw_cn_string("返回");
        }
    }
    else
    {
        if (button_down == MENU_START)
        {
            glColor3f(1.0f, 1.0f, 1.0f);
        }
        else
        {
            glColor3f(1.0f, 1.0f, 0.0f);
        }
        glRasterPos2f(-0.58f, 0.22f);
        draw_cn_string("开始游戏");

        if (button_down == MENU_RULES)
        {
            glColor3f(1.0f, 1.0f, 1.0f);
        }
        else
        {
            glColor3f(1.0f, 1.0f, 0.0f);
        }
        glRasterPos2f(-0.58f, 0.02f);
        draw_cn_string("游戏规则");

        if (button_down == MENU_SETTINGS)
        {
            glColor3f(1.0f, 1.0f, 1.0f);
        }
        else
        {
            glColor3f(1.0f, 1.0f, 0.0f);
        }
        glRasterPos2f(-0.58f, -0.18f);
        draw_cn_string("游戏设置");

        if (button_down == MENU_TEAM)
        {
            glColor3f(1.0f, 1.0f, 1.0f);
        }
        else
        {
            glColor3f(1.0f, 1.0f, 0.0f);
        }
        glRasterPos2f(-0.58f, -0.38f);
        draw_cn_string("制作人员");

        if (button_down == MENU_QUIT)
        {
            glColor3f(1.0f, 1.0f, 1.0f);
        }
        else
        {
            glColor3f(1.0f, 1.0f, 0.0f);
        }
        glRasterPos2f(-0.49f, -0.58f);
        draw_cn_string("退出");

        if (flash_done)
        {
            --tick;
            if (tick <= 0)
            {
                flash_done = 0;
            }
        }

        draw_mini_chessboard(tick);
    }
}

void draw_mini_chessboard(long tick)
{
    // (x+0.1)/4+0.5  y/4

    glBegin(GL_POLYGON);        // to draw mini chessboard
    {
        glColor3f(CHESSBOARD_R, CHESSBOARD_G, CHESSBOARD_B);
        glVertex2f(MINI_FLASH(0.3f, 0.2f));
        glVertex2f(MINI_FLASH(0.7f, 0.2f));
        glVertex2f(MINI_FLASH(0.7f, -0.2f));
        glVertex2f(MINI_FLASH(0.3f, -0.2f));

    }
    glEnd();

    glLineWidth(1.0);           // to draw mini chessboard lines
    glBegin(GL_LINES);
    {
        for (int loop = 0; loop < CHESSBOARD_LINE_NUMBER; ++loop)
        {
            glColor3f(0.0f, 0.0f, 0.0f);
            glVertex2f(MINI_FLASH((CHESSBOARD_COLUMN_START+0.1)/4.0f+0.5, (CHESSBOARD_ROW_START+CHESSBOARD_ROW_STEP*loop)/4.0f));
            glVertex2f(MINI_FLASH((CHESSBOARD_COLUMN_END+0.1)/4.0f+0.5, (CHESSBOARD_ROW_START+CHESSBOARD_ROW_STEP*loop)/4.0f));
            glColor3f(0.0f, 0.0f, 0.0f);
            glVertex2f(MINI_FLASH((CHESSBOARD_COLUMN_START+CHESSBOARD_COLUMN_STEP*loop+0.1)/4.0f+0.5, CHESSBOARD_ROW_START/4.0f));
            glVertex2f(MINI_FLASH((CHESSBOARD_COLUMN_START+CHESSBOARD_COLUMN_STEP*loop+0.1)/4.0f+0.5, CHESSBOARD_ROW_END/4.0f));
        }
    }
    glEnd();

    glColor3f(0.0f, 0.0f, 0.0f);             // to draw mini chessboard points
    draw_solid_circle(MINI_FLASH(0.5f, 0.0f), 0.012f*tick/FLASH_FRAME_NUMBER, 10);
    draw_solid_circle(MINI_FLASH(0.5f, 0.1f), 0.012f*tick/FLASH_FRAME_NUMBER, 10);
    draw_solid_circle(MINI_FLASH(0.5f, -0.1f), 0.012f*tick/FLASH_FRAME_NUMBER, 10);
    draw_solid_circle(MINI_FLASH(0.4f, 0.0f), 0.012f*tick/FLASH_FRAME_NUMBER, 10);
    draw_solid_circle(MINI_FLASH(0.6f, 0.0f), 0.012f*tick/FLASH_FRAME_NUMBER, 10);
    draw_solid_circle(MINI_FLASH(0.4f, 0.1f), 0.012f*tick/FLASH_FRAME_NUMBER, 10);
    draw_solid_circle(MINI_FLASH(0.6f, 0.1f), 0.012f*tick/FLASH_FRAME_NUMBER, 10);
    draw_solid_circle(MINI_FLASH(0.4f, -0.1f), 0.012f*tick/FLASH_FRAME_NUMBER, 10);
    draw_solid_circle(MINI_FLASH(0.6f, -0.1f), 0.012f*tick/FLASH_FRAME_NUMBER, 10);
}

int evaluate_check(int chess_side, int start_x, int start_y, int count, ...)    // 0 45 90 135
{
    int successful_match = 0, chess_side_to_match, match_flag;
    va_list arg_list;
    static const int dir[4][2] = {{0, 1}, {-1, 1}, {1, 0}, {1, 1}};

    for (int degree = 0; degree < 4; ++degree)      // 0, 1, 2, 3 stand for 0, 45, 90, 135 degree
    {
        if (start_x+(count-1)*dir[degree][0] < -1 || start_x+(count-1)*dir[degree][0] > CHESSBOARD_LINE_NUMBER
            || start_y+(count-1)*dir[degree][1] < -1 || start_y+(count-1)*dir[degree][1] > CHESSBOARD_LINE_NUMBER)
        {
            continue;
        }

        match_flag = 1;
        va_start(arg_list, count);
        for (int loop = 1; loop <= count; ++loop)
        {
            chess_side_to_match = va_arg(arg_list, int);

            if (loop == 1 && (start_x == -1 || start_x == CHESSBOARD_LINE_NUMBER
                              || start_y == -1 || start_y == CHESSBOARD_LINE_NUMBER))
            {
                if (chess_side_to_match != -chess_side)
                {
                    match_flag = 0;
                    break;
                }
            }
            else if (loop == count && (start_x+(count-1)*dir[degree][0] == -1 || start_x+(count-1)*dir[degree][0] == CHESSBOARD_LINE_NUMBER
                                       || start_y+(count-1)*dir[degree][1] == -1 || start_y+(count-1)*dir[degree][1] == CHESSBOARD_LINE_NUMBER))
            {
                if (chess_side_to_match != -chess_side)
                {
                    match_flag = 0;
                    break;
                }
            }

            if (chessboard[start_x+(loop-1)*dir[degree][0]][start_y+(loop-1)*dir[degree][1]] != chess_side_to_match)
            {
                match_flag = 0;
                break;
            }

            // avoid double counting
            /*if (loop == 1 && chess_side_to_match
                && start_x-dir[degree][0] >= 0 && start_x-dir[degree][0] < CHESSBOARD_LINE_NUMBER
                && start_y-dir[degree][1] >= 0 && start_y-dir[degree][1] < CHESSBOARD_LINE_NUMBER
                && chessboard[start_x-dir[degree][0]][start_y-dir[degree][1]] != -chess_side_to_match)
            {
                match_flag = 0;
                break;
            }

            if (loop == count && chess_side_to_match
                && start_x+count*dir[degree][0] >= 0 && start_x+count*dir[degree][0] < CHESSBOARD_LINE_NUMBER
                && start_y+count*dir[degree][1] >= 0 && start_y+count*dir[degree][1] < CHESSBOARD_LINE_NUMBER
                && chessboard[start_x+count*dir[degree][0]][start_y+count*dir[degree][1]] != -chess_side_to_match)
            {
                match_flag = 0;
                break;
            }*/
            // avoid double counting
        }
        va_end(arg_list);
        if (match_flag)
        {
            ++successful_match;
        }
    }
//    if (successful_match)
//    {
//        printf("%d %d %d ", start_x, start_y, count);
//        va_start(arg_list, count);
//        for (int loop = 1; loop <= count; ++loop)
//        {
//            chess_side_to_match = va_arg(arg_list, int);
//            printf("%d", abs(chess_side_to_match));
//        }
//        va_end(arg_list);
//        printf(" %d\n", successful_match);
//    }
    return successful_match;
}

long evaluation(int chess_side)
{
    int killer_count = 0, single_killer;
    long sum = 0;
    for (int loop_i = -1; loop_i <= CHESSBOARD_LINE_NUMBER; ++loop_i)
    {
        for (int loop_j = -1; loop_j <= CHESSBOARD_LINE_NUMBER; ++loop_j)
        {
            // in fact, binary system can be well used to avoid number-uncertain arguments definition,
            // but I`m too lazy to tranlate them between binary system and decimal system
            sum += 3000000*evaluate_check(chess_side, loop_i, loop_j, 5, chess_side, chess_side, chess_side, chess_side, chess_side); // 11111

            single_killer = evaluate_check(chess_side, loop_i, loop_j, 6, 0, chess_side, chess_side, chess_side, chess_side, 0); // 011110
            sum += 30000*single_killer;
            killer_count += single_killer;

            single_killer = evaluate_check(chess_side, loop_i, loop_j, 6, -chess_side, chess_side, chess_side, chess_side, chess_side, 0); // X11110
            sum += 1000*single_killer;
            killer_count += single_killer;

            single_killer = evaluate_check(chess_side, loop_i, loop_j, 6, 0, chess_side, chess_side, chess_side, chess_side, -chess_side); // 01111X
            sum += 1000*single_killer;
            killer_count += single_killer;

            single_killer = evaluate_check(chess_side, loop_i, loop_j, 5, chess_side, chess_side, chess_side, 0, chess_side); // 11101
            sum += 900*single_killer;
            killer_count += single_killer;

            single_killer = evaluate_check(chess_side, loop_i, loop_j, 5, chess_side, 0, chess_side, chess_side, chess_side); // 10111
            sum += 900*single_killer;
            killer_count += single_killer;

            single_killer = evaluate_check(chess_side, loop_i, loop_j, 5, chess_side, chess_side, 0, chess_side, chess_side); // 11011
            sum += 900*single_killer;
            killer_count += single_killer;

            sum += 10*evaluate_check(chess_side, loop_i, loop_j, 6, -chess_side, chess_side, chess_side, chess_side, chess_side, -chess_side); // X1111X

            single_killer = evaluate_check(chess_side, loop_i, loop_j, 7, 0, 0, chess_side, chess_side, chess_side, 0, 0); // 0011100
            sum += 800*single_killer;
            killer_count += single_killer;

            single_killer = evaluate_check(chess_side, loop_i, loop_j, 7, -chess_side, 0, chess_side, chess_side, chess_side, 0, 0); // X011100
            sum += 600*single_killer;
            killer_count += single_killer;

            single_killer = evaluate_check(chess_side, loop_i, loop_j, 7, 0, 0, chess_side, chess_side, chess_side, 0, -chess_side); // 001110X
            sum += 600*single_killer;
            killer_count += single_killer;

            single_killer = evaluate_check(chess_side, loop_i, loop_j, 6, 0, chess_side, 0, chess_side, chess_side, 0); // 010110
            sum += 100*single_killer;
            killer_count += single_killer;

            single_killer = evaluate_check(chess_side, loop_i, loop_j, 6, 0, chess_side, chess_side, 0, chess_side, 0); // 011010
            sum += 100*single_killer;
            killer_count += single_killer;

            sum += 50*evaluate_check(chess_side, loop_i, loop_j, 6, -chess_side, chess_side, chess_side, chess_side, 0, 0); // X11100
            sum += 50*evaluate_check(chess_side, loop_i, loop_j, 6, 0, 0, chess_side, chess_side, chess_side, -chess_side); // 00111X
            sum += 60*evaluate_check(chess_side, loop_i, loop_j, 5, chess_side, 0, 0, chess_side, chess_side); // 10011
            sum += 60*evaluate_check(chess_side, loop_i, loop_j, 5, chess_side, chess_side, 0, 0, chess_side); // 11001
            sum += 55*evaluate_check(chess_side, loop_i, loop_j, 5, chess_side, 0, chess_side, 0, chess_side); // 10101
            sum += 50*evaluate_check(chess_side, loop_i, loop_j, 7, -chess_side, 0, chess_side, chess_side, chess_side, 0, -chess_side); // X01110X
            sum += 5*evaluate_check(chess_side, loop_i, loop_j, 4, chess_side, chess_side, 0, chess_side); // 1101
            sum += 5*evaluate_check(chess_side, loop_i, loop_j, 4, chess_side, 0, chess_side, chess_side); // 1011
            sum += 5*evaluate_check(chess_side, loop_i, loop_j, 5, -chess_side, chess_side, chess_side, chess_side, 0); // X1110
            sum += 5*evaluate_check(chess_side, loop_i, loop_j, 5, 0, chess_side, chess_side, chess_side, -chess_side); // 0111X
            sum += 5*evaluate_check(chess_side, loop_i, loop_j, 5, -chess_side, chess_side, chess_side, chess_side, -chess_side); // X111X
            sum += 65*evaluate_check(chess_side, loop_i, loop_j, 8, 0, 0, 0, chess_side, chess_side, 0, 0, 0); // 00011000
            sum += 50*evaluate_check(chess_side, loop_i, loop_j, 8, -chess_side, 0, 0, chess_side, chess_side, 0, 0, 0); // X0011000
            sum += 50*evaluate_check(chess_side, loop_i, loop_j, 8, 0, 0, 0, chess_side, chess_side, 0, 0, -chess_side); // 0001100X
            sum += 35*evaluate_check(chess_side, loop_i, loop_j, 8, -chess_side, 0, 0, chess_side, chess_side, 0, 0, -chess_side); // X001100X
            sum += 20*evaluate_check(chess_side, loop_i, loop_j, 6, -chess_side, 0, chess_side, chess_side, 0, 0); // X01100
            sum += 20*evaluate_check(chess_side, loop_i, loop_j, 6, 0, 0, chess_side, chess_side, 0, -chess_side); // 00110X
            sum += 10*evaluate_check(chess_side, loop_i, loop_j, 6, -chess_side, 0, chess_side, chess_side, 0, -chess_side); // X0110X
            sum += 25*evaluate_check(chess_side, loop_i, loop_j, 7, 0, 0, chess_side, 0, chess_side, 0, 0); // 0010100
            sum += 10*evaluate_check(chess_side, loop_i, loop_j, 7, -chess_side, 0, chess_side, 0, chess_side, 0, 0); // X010100
            sum += 10*evaluate_check(chess_side, loop_i, loop_j, 7, 0, 0, chess_side, 0, chess_side, 0, -chess_side); // 001010X
            sum += 20*evaluate_check(chess_side, loop_i, loop_j, 8, -chess_side, 0, chess_side, 0, 0, chess_side, 0, -chess_side); // X010010X
            sum += 15*evaluate_check(chess_side, loop_i, loop_j, 6, -chess_side, chess_side, chess_side, 0, 0, 0); // X11000
            sum += 15*evaluate_check(chess_side, loop_i, loop_j, 6, 0, 0, 0, chess_side, chess_side, -chess_side); // 00011X
            sum += 15*evaluate_check(chess_side, loop_i, loop_j, 6, -chess_side, chess_side, 0, chess_side, 0, 0); // X10100
            sum += 15*evaluate_check(chess_side, loop_i, loop_j, 6, 0, 0, chess_side, 0, chess_side, -chess_side); // 00101X
        }
    }
    if (killer_count >= 2)
    {
        sum += 15000*killer_count;
    }

    return sum;
}

int search_check(int check_x, int check_y)
{
    if (check_x < 0 || check_x >= CHESSBOARD_LINE_NUMBER || check_y < 0 || check_y >= CHESSBOARD_LINE_NUMBER)
    {
        return 0;
    }

    if (chessboard[check_x][check_y])
    {
        return 0;
    }

    for (int loop_i = -2; loop_i < 3; ++loop_i)
    {
        for (int loop_j = -2; loop_j < 3; ++loop_j)
        {
            if (check_x+loop_i >= 0 && check_x+loop_i < CHESSBOARD_LINE_NUMBER
                && check_y+loop_j >= 0 && check_y+loop_j < CHESSBOARD_LINE_NUMBER
                && chessboard[check_x+loop_i][check_y+loop_j])
            {
                return 1;
            }
        }
    }
    return 0;
}

int search_count()
{
    int count = 0;
    for (int loop_i = 0; loop_i < CHESSBOARD_LINE_NUMBER; ++loop_i)
    {
        for (int loop_j = 0; loop_j < CHESSBOARD_LINE_NUMBER; ++loop_j)
        {
            if (search_check(loop_i, loop_j))
            {
                ++count;
            }
        }
    }
    return count;
}

void draw_cn_string(const char* str)
{
    int len;
    wchar_t* wstring;
    HDC hDC = wglGetCurrentDC();
    GLuint list = glGenLists(1);

    // count the number of char
    // if one occupies two bytes, then it will be called a double-byte char (wide char)
    // or a single byte char
    len = 0;
    for(int loop = 0; str[loop] != '\0'; ++loop)
    {
        if(IsDBCSLeadByte(str[loop]))
        {
            ++loop;
        }
        ++len;
    }

    // transform mixed char into wide char
    wstring = (wchar_t*)malloc((len+1)*sizeof(wchar_t));
    MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, str, -1, wstring, len);
    wstring[len] = L'\0';

    // output one by one
    for(int loop = 0; loop < len; ++loop)
    {
        wglUseFontBitmapsW(hDC, wstring[loop], 1, list);
        glCallList(list);
    }

    // recycle
    free(wstring);
    glDeleteLists(list, 1);
}

void selectFont(int size, int charset, const char* face)
{
    HFONT hFont = CreateFontA(size, 0, 0, 0, FW_MEDIUM, 0, 0, 0,
                              charset, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                              DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, face);
    HFONT hOldFont = (HFONT)SelectObject(wglGetCurrentDC(), hFont);
    DeleteObject(hOldFont);
}

//void draw_buttons()
//{
//    glClear(GL_COLOR_BUFFER_BIT);
//
//    selectFont(48, ANSI_CHARSET, "Comic Sans MS");
//    glColor3f(1.0f, 0.0f, 0.0f);
//    glRasterPos2f(-0.7f, 0.4f);
//    draw_cn_string("Hello, World!");
//
//    selectFont(48, GB2312_CHARSET, "楷体_GB2312");
//    glColor3f(1.0f, 1.0f, 0.0f);
//    glRasterPos2f(-0.7f, -0.1f);
//    draw_cn_string("当代的中国汉字");
//
//    selectFont(48, DEFAULT_CHARSET, "华文仿宋");
//    glColor3f(0.0f, 1.0f, 0.0f);
//    glRasterPos2f(-0.7f, -0.6f);
//    draw_cn_string("傳統的中國漢字");
//
//    glutSwapBuffers();
//}

int check_button(int x, int y)
{
    if (menu_choice && sub_menu_choice && difficult_choice && x >= 570 && x <= 600 && y >= 120 && y <= 180)
    {
        return GAME_UNDO;
    }

    if (menu_choice && sub_menu_choice && difficult_choice && x >= 570 && x <= 600 && y >= 270 && y <= 330)
    {
        return GAME_RESTART;
    }

    if (menu_choice && sub_menu_choice && difficult_choice && x >= 570 && x <= 600 && y >= 420 && y <= 480)
    {
        return GAME_RETURN;
    }

    if (!menu_choice && x >= 120 && x <= 240 && y >= 210 && y <= 240)
    {
        return MENU_START;
    }

    if (!menu_choice && x >= 120 && x <= 240 && y >= 270 && y <= 300)
    {
        return MENU_RULES;
    }

    if (!menu_choice && x >= 120 && x <= 240 && y >= 330 && y <= 360)
    {
        return MENU_SETTINGS;
    }

    if (!menu_choice && x >= 120 && x <= 240 && y >= 390 && y <= 420)
    {
        return MENU_TEAM;
    }

    if (!menu_choice && x >= 150 && x <= 210 && y >= 450 && y <= 480)
    {
        return MENU_QUIT;
    }

    if (menu_choice && !sub_menu_choice && x >= 120 && x <= 240 && y >= 210 && y <= 240)
    {
        return SUB_MENU_HUMAN;
    }

    if (menu_choice && !sub_menu_choice && x >= 120 && x <= 240 && y >= 270 && y <= 300)
    {
        return SUB_MENU_COMPUTER;
    }

    if (menu_choice && sub_menu_choice && !difficult_choice && x >= 120 && x <= 240 && y >= 210 && y <= 240)
    {
        return DIFFICULT_EASY;
    }

    if (menu_choice && sub_menu_choice && !difficult_choice && x >= 120 && x <= 240 && y >= 270 && y <= 330)
    {
        return DIFFICULT_NORMAL;
    }

    if (menu_choice && sub_menu_choice && !difficult_choice && x >= 120 && x <= 240 && y >= 330 && y <= 360)
    {
        return DIFFICULT_HARD;
    }

    if (menu_choice && sub_menu_choice && !difficult_choice && x >= 120 && x <= 240 && y >= 390 && y <= 420)
    {
        return DIFFICULT_HELL;
    }

    if (menu_choice && !sub_menu_choice && x >= 150 && x <= 210 && y >= 450 && y <= 480)
    {
        return SUB_MENU_RETURN;
    }

    if (menu_choice && sub_menu_choice && !difficult_choice && x >= 150 && x <= 210 && y >= 450 && y <= 480)
    {
        return DIFFICULT_RETURN;
    }

    if (menu_choice && sub_menu_choice && difficult_choice && !side_choice && x >= 120 && x <= 240 && y >= 210 && y <= 240)
    {
        return SIDE_BLACK;
    }

    if (menu_choice && sub_menu_choice && difficult_choice && !side_choice && x >= 120 && x <= 240 && y >= 270 && y <= 330)
    {
        return SIDE_WHITE;
    }

    if (menu_choice && sub_menu_choice && difficult_choice && !side_choice && x >= 150 && x <= 210 && y >= 450 && y <= 480)
    {
        return SIDE_RETURN;
    }

    if (menu_choice == MENU_SETTINGS && !sub_menu_choice && x >= 300 && x <= 550 && y >= 210 && y <= 240)
    {
        return SETTINGS_CHESS_RAIN;
    }

    return 0;
}

void menu_button_down_judgement(int down_x, int down_y, int x, int y)
{
    int button;

    if ((button = check_button(down_x, down_y)) == check_button(x, y))
    {
        if (button >= MENU_START && button < MENU_QUIT)
        {
            menu_choice = button;
        }
        else if (button == MENU_QUIT)
        {
            PostQuitMessage(0);
        }
        else if (button == SUB_MENU_HUMAN)
        {
            sub_menu_choice = button;
            difficult_choice = side_choice = -1;
        }
        else if (button == SUB_MENU_COMPUTER)
        {
            sub_menu_choice = button;
        }
        else if (button == DIFFICULT_EASY)
        {
            difficult_choice = button;
            computer_defense = 1.0f;
            computer_depth = 1;
        }
        else if (button == DIFFICULT_NORMAL)
        {
            difficult_choice = button;
            computer_defense = 1.5f;
            computer_depth = 1;
        }
        else if (button == DIFFICULT_HARD)
        {
            difficult_choice = button;
            computer_defense = 1.0f;
            computer_depth = 2;
        }
        else if (button == DIFFICULT_HELL)
        {
            difficult_choice = button;
            computer_defense = 1.0f;
            computer_depth = 3;
        }
        else if (button == SUB_MENU_RETURN)
        {
            menu_choice = 0;
        }
        else if (button == DIFFICULT_RETURN)
        {
            sub_menu_choice = 0;
        }
        else if (button == SIDE_BLACK || button == SIDE_WHITE)
        {
            side_choice = button;
            human_side = side_choice == SIDE_BLACK ? BLACK_CHESS : WHITE_CHESS;
        }
        else if (button == SIDE_RETURN)
        {
            difficult_choice = 0;
        }
        else if (button == SETTINGS_CHESS_RAIN)
        {
            chess_rain_enable = 1 - chess_rain_enable;
        }
    }
}

void return_to_menu_judgement(int down_x, int down_y, int x, int y)
{
    if (x >= 570 && x <= 600 && y >= 420 && y <= 480 && down_x >= 570 && down_x <= 600 && down_y >= 420 && down_y <= 480)
    {
        restart();
        menu_choice = sub_menu_choice = difficult_choice = side_choice = human_side = 0;
    }
}

void draw_chess_rain()
{
    static int randomrize_flag = 0;
    static struct rain_node rain_record[MAX_CHESS_RAIN_NUMBER];

    if (!randomrize_flag)
    {
        srand(time(0));
        randomrize_flag = 1;
        rain_record[0].position_y = rain_record[1].position_y = rain_record[2].position_y =
        rain_record[3].position_y = rain_record[4].position_y = rain_record[5].position_y = -1.1f;
    }

    for (int loop = 0; loop < MAX_CHESS_RAIN_NUMBER; ++loop)
    {
        if (rain_record[loop].position_y <= -1.05f
            && (rand()%CHESS_RAIN_GENERATING_PROBABILITY_INDEX+1) <= CHESS_RAIN_GENERATING_PROBABILITY_INDEX*CHESS_RAIN_GENERATING_PROBABILITY)
        {
            rain_record[loop].speed = (float)(rand()%(MAX_CHESS_RAIN_SPEED-MIN_CHESS_RAIN_SPEED+1)+MIN_CHESS_RAIN_SPEED)/CHESS_RAIN_SPEED_INDEX;
            rain_record[loop].length = rand()%(MAX_CHESS_RAIN_LENGTH-MIN_CHESS_RAIN_LENGTH+1)+MIN_CHESS_RAIN_LENGTH;
            rain_record[loop].position_y = 1.05f+(float)rain_record[loop].length/10;
            rain_record[loop].distribution = rand()%(1<<rain_record[loop].length);
            rain_record[loop].position_x = (float)(rand()%(MAX_CHESS_RAIN_X_POSITION-MIN_CHESS_RAIN_X_POSITION+1)+MIN_CHESS_RAIN_X_POSITION)/CHESS_RAIN_X_POSITION_INDEX;
            //printf("fuck\n");
            //printf("%f %d %f %d %f\n", rain_record[loop].speed, rain_record[loop].length, rain_record[loop].position_y, rain_record[loop].distribution, rain_record[loop].position_x);
        }

        glLineWidth(2.0);
        glColor3f(1.0f, 1.0f, 1.0f);
        for (int sub_loop = 0; sub_loop < rain_record[loop].length; ++sub_loop)
        {
            if ((1<<sub_loop)&(rain_record[loop].distribution))
            {
                draw_solid_circle(rain_record[loop].position_x, rain_record[loop].position_y-(float)sub_loop/10.0f, 0.05f, 100);
            }
            else
            {
                draw_wired_circle(rain_record[loop].position_x, rain_record[loop].position_y-(float)sub_loop/10.0f, 0.05f, 100);
            }
            //printf("shit\n");
            //printf("%f %d %f %d %f\n", rain_record[loop].speed, rain_record[loop].length, rain_record[loop].position_y, rain_record[loop].distribution, rain_record[loop].position_x);
        }

        rain_record[loop].position_y -= rain_record[loop].speed;
    }
}

void draw_ratio(int whether_enable, float ratio_x, float ratio_y)
{
    glLineWidth(2.0);
    glBegin(GL_LINES);
    {
        glColor3f(1.0f, 1.0f, 1.0f);
        glVertex2f(ratio_x-0.04f, ratio_y-0.04f);
        glVertex2f(ratio_x+0.04f, ratio_y-0.04f);
        glColor3f(1.0f, 1.0f, 1.0f);
        glVertex2f(ratio_x+0.04f, ratio_y-0.04f);
        glVertex2f(ratio_x+0.04f, ratio_y+0.04f);
        glColor3f(1.0f, 1.0f, 1.0f);
        glVertex2f(ratio_x+0.04f, ratio_y+0.04f);
        glVertex2f(ratio_x-0.04f, ratio_y+0.04f);
        glColor3f(1.0f, 1.0f, 1.0f);
        glVertex2f(ratio_x-0.04f, ratio_y+0.04f);
        glVertex2f(ratio_x-0.04f, ratio_y-0.04f);
    }
    glEnd();

    if (whether_enable)
    {
        glPointSize(20);
        glBegin(GL_POINTS);
        {
            glColor3f(1.0f, 1.0f, 1.0f);
            glVertex2f(ratio_x, ratio_y);
        }
        glEnd();
    }
}
