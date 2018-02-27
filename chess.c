#include "gobang_lib.h"

extern int chessboard[CHESSBOARD_LINE_NUMBER][CHESSBOARD_LINE_NUMBER];
extern int turn, game_abort, turn_number, win_side, brain_turn, brain_turn_number;
extern int win_sign_start_x, win_sign_start_y, win_sign_end_x, win_sign_end_y;
extern int chess_sign, chess_sign_x, chess_sign_y, human_side;
extern struct chess_node chess_order[CHESSBOARD_LINE_NUMBER*CHESSBOARD_LINE_NUMBER+1];

int method_x, method_y, method_backup_flag, method_x_backup, method_y_backup, max_depth_backup, win_flag;

void chess_judgement(int down_x, int down_y, int x, int y, int chess_kind)
{
    for (int loop_i = 0; loop_i < CHESSBOARD_LINE_NUMBER; ++loop_i)
    {
        for (int loop_j = 0; loop_j < CHESSBOARD_LINE_NUMBER; ++loop_j)
        {
            if (!chessboard[loop_i][loop_j])
            {
                if (CHECK_WITHIN_CHESS_RADIUS(down_x, down_y) && CHECK_WITHIN_CHESS_RADIUS(x, y))
                {
                    if (chess_kind == REAL_CHESS)
                    {
                        chessboard[loop_i][loop_j] = turn;
                        turn = -turn;       // equal to "if CHESS_BLACK then CHESS_WHITE else CHESS_BLACK"
                        chess_order[++turn_number].x = loop_i;
                        chess_order[turn_number].y = loop_j;
                        win_side = check_win(loop_i, loop_j);
                        brain_turn = turn;        // to distinct real chess between brain chess
                        brain_turn_number = turn_number;
                    }
                    else if (chess_kind == BRAIN_CHESS)
                    {
                        chessboard[loop_i][loop_j] = brain_turn;
                        brain_turn = -brain_turn;       // equal to "if CHESS_BLACK then CHESS_WHITE else CHESS_BLACK"
                        chess_order[++brain_turn_number].x = loop_i;
                        chess_order[brain_turn_number].y = loop_j;
                    }

                    // due to the two coordinates are upside down, and what`s more, they are oppsite in x and y.
                    //printf("Chess at [%d][%d] of %s chess\n", 14-loop_j, loop_i, turn < 0 ? "white" : "black");
                    printf("Chess at [%d][%d] of %s chess\n", loop_i, loop_j, turn < 0 ? "white" : "black");
                    printf("Black : %ld\n", evaluation(BLACK_CHESS));
                    printf("White : %ld\n\n", evaluation(WHITE_CHESS));
                }
            }
        }
    }

    // well, because two coordinates are upside down and what`s more, they are oppsite in x and y.
    // so when it comes to rows, it should be "ROW_END-" while COLUMN`s is "COLUMN_START+" in the macro definition
}

void undo_judgement(int down_x, int down_y, int x, int y)
{
    if (x >= 570 && x <= 600 && y >= 120 && y <= 180 && down_x >= 570 && down_x <= 600 && down_y >= 120 && down_y <= 180)
    {
        if (human_side)
        {
            if (turn_number > 0)
            {
                chessboard[chess_order[turn_number].x][chess_order[turn_number].y] = 0;
                --turn_number;
                chessboard[chess_order[turn_number].x][chess_order[turn_number].y] = 0;
                --turn_number;
                brain_turn_number = turn_number;
            }
        }
        else
        {
            if (turn_number > 0)
            {
                chessboard[chess_order[turn_number].x][chess_order[turn_number].y] = 0;
                --turn_number;
                turn = -turn;
                brain_turn = turn;        // to distinct real chess between brain chess
                brain_turn_number = turn_number;
            }
        }
    }
}

int check_win(int chess_x, int chess_y)
{
    if (turn_number == CHESSBOARD_LINE_NUMBER*CHESSBOARD_LINE_NUMBER)
    {
        return TIE;
    }

    for (int loop_j = MAX(0, chess_y-4); loop_j <= MIN(CHESSBOARD_LINE_NUMBER-5, chess_y); ++loop_j)    // 0 degree
    {
        if (abs(chessboard[chess_x][loop_j]+chessboard[chess_x][loop_j+1]+chessboard[chess_x][loop_j+2]
            +chessboard[chess_x][loop_j+3]+chessboard[chess_x][loop_j+4]) == 5)
        {
            win_sign_start_x = chess_x;
            win_sign_start_y = loop_j;
            win_sign_end_x = chess_x;
            win_sign_end_y = loop_j+4;

            if (chessboard[chess_x][chess_y] > 0)
            {
                return WHITE_CHESS;
            }
            else
            {
                return BLACK_CHESS;
            }
        }
    }

    for (int loop_i = MAX(0, chess_x-4); loop_i <= MIN(CHESSBOARD_LINE_NUMBER-5, chess_x); ++loop_i)    // 90 degree
    {
        if (abs(chessboard[loop_i][chess_y]+chessboard[loop_i+1][chess_y]+chessboard[loop_i+2][chess_y]
            +chessboard[loop_i+3][chess_y]+chessboard[loop_i+4][chess_y]) == 5)
        {
            win_sign_start_x = loop_i;
            win_sign_start_y = chess_y;
            win_sign_end_x = loop_i+4;
            win_sign_end_y = chess_y;

            if (chessboard[chess_x][chess_y] > 0)
            {
                return WHITE_CHESS;
            }
            else
            {
                return BLACK_CHESS;
            }
        }
    }

    for (int loop = MAX(-MIN(chess_x, chess_y), -4); loop <= MIN(CHESSBOARD_LINE_NUMBER-5-MAX(chess_x, chess_y), 0); ++loop)    // 135 degree
    {
        if (abs(chessboard[chess_x+loop][chess_y+loop]+chessboard[chess_x+loop+1][chess_y+loop+1]+chessboard[chess_x+loop+2][chess_y+loop+2]
            +chessboard[chess_x+loop+3][chess_y+loop+3]+chessboard[chess_x+loop+4][chess_y+loop+4]) == 5)
        {
            win_sign_start_x = chess_x+loop;
            win_sign_start_y = chess_y+loop;
            win_sign_end_x = chess_x+loop+4;
            win_sign_end_y = chess_y+loop+4;

            if (chessboard[chess_x][chess_y] > 0)
            {
                return WHITE_CHESS;
            }
            else
            {
                return BLACK_CHESS;
            }
        }
    }

    for (int loop = MAX(-MIN(CHESSBOARD_LINE_NUMBER-1-chess_x, chess_y), -4); loop <= MIN(MIN(chess_x-4, CHESSBOARD_LINE_NUMBER-5-chess_y), 0); ++loop)    // 45 degree
    {
        if (abs(chessboard[chess_x-loop][chess_y+loop]+chessboard[chess_x-loop-1][chess_y+loop+1]+chessboard[chess_x-loop-2][chess_y+loop+2]
            +chessboard[chess_x-loop-3][chess_y+loop+3]+chessboard[chess_x-loop-4][chess_y+loop+4]) == 5)
        {
            win_sign_start_x = chess_x-loop;
            win_sign_start_y = chess_y+loop;
            win_sign_end_x = chess_x-loop-4;
            win_sign_end_y = chess_y+loop+4;

            if (chessboard[chess_x][chess_y] > 0)
            {
                return WHITE_CHESS;
            }
            else
            {
                return BLACK_CHESS;
            }
        }
    }

    return 0;
}

void chessable_judgement(int x, int y)
{
    for (int loop_i = 0; loop_i < CHESSBOARD_LINE_NUMBER; ++loop_i)
    {
        for (int loop_j = 0; loop_j < CHESSBOARD_LINE_NUMBER; ++loop_j)
        {
            if (!chessboard[loop_i][loop_j])
            {
                if (CHECK_WITHIN_CHESS_RADIUS(x, y))
                {
                    chess_sign = 1;
                    chess_sign_x = loop_i;
                    chess_sign_y = loop_j;
                    return;
                }
            }
        }
    }
    chess_sign = 0;

    // well, because two coordinates are upside down, so when it comes to rows,
    // it should be "ROW_END-" while COLUMN`s is "COLUMN_START+"
}

void restart()
{
    for (int loop_i = 0; loop_i < CHESSBOARD_LINE_NUMBER; ++loop_i)
    {
        for (int loop_j = 0; loop_j < CHESSBOARD_LINE_NUMBER; ++loop_j)
        {
            chessboard[loop_i][loop_j] = 0;
        }
    }

    turn_number = 0;
    turn = -1;
    brain_turn = turn;        // to distinct real chess between brain chess
    brain_turn_number = turn_number;
    game_abort = 0;
    win_side = 0;

    // to alert to choose the first to chess
}

void restart_judgement(int down_x, int down_y, int x, int y)
{
    if (x >= 570 && x <= 600 && y >= 270 && y <= 330 && down_x >= 570 && down_x <= 600 && down_y >= 270 && down_y <= 330)
    {
        restart();
    }
}

void brain_chess_undo()
{
    if (brain_turn_number > turn_number)
    {
        chessboard[chess_order[brain_turn_number].x][chess_order[brain_turn_number].y] = 0;
        --brain_turn_number;
        brain_turn = -brain_turn;
    }
}

void brain_chess_deactivate()
{
    while (brain_turn_number > turn_number)
    {
        brain_chess_undo();
    }
}

FILE *fout;

void computer_chess(int computer_side, int max_depth, float defense_coefficient)
{
    fout = fopen("F:\\Assignment Experiment\\Gobang\\Pythooooooooooooooooooooooon.txt", "w");
    fprintf(fout, "ROUND : %d\n\n", turn_number+1);
    win_flag = 0;
    max_depth_backup = max_depth;
    long best_value;

    if (!turn_number)
    {
        chessboard[CHESSBOARD_LINE_NUMBER/2][CHESSBOARD_LINE_NUMBER/2] = turn;
        turn = -turn;
        chess_order[++turn_number].x = CHESSBOARD_LINE_NUMBER/2;
        chess_order[turn_number].y = CHESSBOARD_LINE_NUMBER/2;
        brain_turn = turn;
        brain_turn_number = turn_number;
    }
    else
    {
        best_value = computer_search(computer_side, defense_coefficient, max_depth, MIN_LONG, MAX_LONG);
        chessboard[method_x][method_y] = turn;
        turn = -turn;
        chess_order[++turn_number].x = method_x;
        chess_order[turn_number].y = method_y;
        win_side = check_win(method_x, method_y);
    }
    brain_turn = turn;
    brain_turn_number = turn_number;
    printf("fuck : %d\n", turn_number);
    fclose(fout);
}

long computer_search(int computer_side, float defense_coefficient, int depth, long alpha, long beta)
{
    if (!depth || win_flag)
    {
        if (!(depth%2))
        {
            return evaluation(computer_side)-defense_coefficient*evaluation(-computer_side);
        }
        else
        {
            return evaluation(-computer_side)-defense_coefficient*evaluation(computer_side);
        }
    }

    long next_value;
    int same_step_used_flag = 1, step_count = 0, step_number = 0;
    int search_done = 0, search_all = search_count();
    int max_x, max_y, min_x, min_y, loop_i, loop_j, direction_number = 3;
    static int direction[4][2] = {{0, -1}, {1, 0}, {0, 1}, {-1, 0}};
    max_x = max_y = -CHESSBOARD_LINE_NUMBER;
    min_x = min_y = CHESSBOARD_LINE_NUMBER;

//    for (int loop_i = 0; loop_i < CHESSBOARD_LINE_NUMBER; ++loop_i)
//    {
//        for (int loop_j = 0; loop_j < CHESSBOARD_LINE_NUMBER; ++loop_j)
//        {
//            if (search_check(loop_i, loop_j))
//            {
//                if (depth == max_depth_backup)
//                {
//                    method_backup_flag = 0;
//                    method_x_backup = method_y_backup = 0;
//                }
//
//                chessboard[loop_i][loop_j] = brain_turn;
//                chess_order[++brain_turn_number].x = loop_i;
//                chess_order[brain_turn_number].y = loop_j;
//                brain_turn = -brain_turn;
//                win_flag = check_win(loop_i, loop_j);
//                fprintf(fout, "before:%4d%14d%14d%4d%4d\n", depth, alpha, beta, loop_i, loop_j);
//                next_value = -computer_search(computer_side, defense_coefficient, depth-1, -beta, -alpha);
//                brain_turn = -brain_turn;
//                --brain_turn_number;
//                chessboard[loop_i][loop_j] = 0;
//                win_flag = 0;
//
//                fprintf(fout, "after :%4d%14d%14d%4d%4d%14d\n", depth, alpha, beta, loop_i, loop_j, next_value);
//                if (depth == max_depth_backup)
//                {
//                    method_backup_flag = 0;
//                    method_x_backup = method_y_backup = 0;
//                }
//
//                if (next_value >= beta)
//                {
//                    if (method_backup_flag)
//                    {
//                        method_backup_flag = 0;
//                        method_x = method_x_backup;
//                        method_y = method_y_backup;
//
//                        fprintf(fout, "nimabi:%4d%4d\n", method_x, method_y);
//                    }
//                    return beta;      // someone thinks it should be "return next_value;"
//                }
//                if (next_value > alpha)
//                {
//                    if (!method_backup_flag)
//                    {
//                        method_backup_flag = 1;
//                        method_x_backup = method_x;
//                        method_y_backup = method_y;
//                    }
//                    alpha = next_value;
//                    method_x = chess_order[turn_number+1].x;
//                    method_y = chess_order[turn_number+1].y;
//
//                    fprintf(fout, "fuck  :%4d%4d%4d%4d%4d%4d\n", depth, method_backup_flag, method_x, method_y, method_x_backup, method_y_backup);
//                }
//            }
//        }
//    }

    for (int loop = 1; loop <= turn_number; ++loop)
    {
        if (chess_order[loop].x > max_x)
        {
            max_x = chess_order[loop].x;
        }
        if (chess_order[loop].x < min_x)
        {
            min_x = chess_order[loop].x;
        }
        if (chess_order[loop].y > max_y)
        {
            max_y = chess_order[loop].y;
        }
        if (chess_order[loop].y < min_y)
        {
            min_y = chess_order[loop].y;
        }
    }
    loop_i = (max_x+min_x)/2;
    loop_j = (max_y+min_y)/2;

    while (search_done < search_all)
    {
        //printf("%d %d %d %d %d %d %d\n", depth, loop_i, loop_j, search_done, search_all, step_count, step_number);
        if (search_check(loop_i, loop_j))
        {
            if (depth == max_depth_backup)
            {
                method_backup_flag = 0;
                method_x_backup = method_y_backup = 0;
            }

            chessboard[loop_i][loop_j] = brain_turn;
            chess_order[++brain_turn_number].x = loop_i;
            chess_order[brain_turn_number].y = loop_j;
            brain_turn = -brain_turn;
            win_flag = check_win(loop_i, loop_j);
            fprintf(fout, "before:%4d%14d%14d%4d%4d\n", depth, alpha, beta, loop_i, loop_j);
            next_value = -computer_search(computer_side, defense_coefficient, depth-1, -beta, -alpha);
            brain_turn = -brain_turn;
            --brain_turn_number;
            chessboard[loop_i][loop_j] = 0;
            win_flag = 0;

            fprintf(fout, "after :%4d%14d%14d%4d%4d%14d\n", depth, alpha, beta, loop_i, loop_j, next_value);
            if (depth == max_depth_backup)
            {
                method_backup_flag = 0;
                method_x_backup = method_y_backup = 0;
            }

            if (next_value >= beta)
            {
                if (method_backup_flag)
                {
                    method_backup_flag = 0;
                    method_x = method_x_backup;
                    method_y = method_y_backup;

                    fprintf(fout, "nimabi:%4d%4d\n", method_x, method_y);
                }
                return beta;      // someone thinks it should be "return next_value;"
            }
            if (next_value > alpha)
            {
                if (!method_backup_flag)
                {
                    method_backup_flag = 1;
                    method_x_backup = method_x;
                    method_y_backup = method_y;
                }
                alpha = next_value;
                method_x = chess_order[turn_number+1].x;
                method_y = chess_order[turn_number+1].y;

                fprintf(fout, "fuck  :%4d%4d%4d%4d%4d%4d\n", depth, method_backup_flag, method_x, method_y, method_x_backup, method_y_backup);
            }

            ++search_done;
        }

        if (step_count == step_number)
        {
            step_count = 0;
            direction_number = (direction_number+1)%4;
            if (!same_step_used_flag)
            {
                same_step_used_flag = 1;
            }
            else
            {
                same_step_used_flag = 0;
                ++step_number;
            }
        }
        loop_i += direction[direction_number][0];
        loop_j += direction[direction_number][1];
        ++step_count;
    }
    return alpha;
}
