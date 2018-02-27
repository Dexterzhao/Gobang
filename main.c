#include "gobang_lib.h"

int chessboard[CHESSBOARD_LINE_NUMBER][CHESSBOARD_LINE_NUMBER];
struct chess_node chess_order[CHESSBOARD_LINE_NUMBER*CHESSBOARD_LINE_NUMBER+1];
int turn = -1, game_abort = 0, turn_number = 0, win_side = 0, brain_turn = -1, brain_turn_number = 0;
int win_sign_start_x, win_sign_start_y, win_sign_end_x, win_sign_end_y;
int chess_sign, chess_sign_x, chess_sign_y, button_down;
int menu_choice, sub_menu_choice, difficult_choice, side_choice, human_side, computer_depth, flash_done;
float computer_defense;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    InitConsoleWindow();    // to use console to output test inf, can be deleted later

    WNDCLASSEX wcex;
    HWND hwnd;
    HDC hDC;
    HGLRC hRC;
    MSG msg;
    BOOL bQuit = FALSE;
    float theta = 0.0f;

    /* register window class */
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_OWNDC;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = "GLSample";
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);;

    /* declare varieties about game logic */
    TCHAR szBuffer[128];

    /* initialization */
    menu_choice = sub_menu_choice = difficult_choice = side_choice = human_side = flash_done = 0;
    restart();

    if (!RegisterClassEx(&wcex))
        return 0;

    /* create main window */
    hwnd = CreateWindowEx(0,
                          "GLSample",
                          "OpenGL Sample",
                          WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME,
                          CW_USEDEFAULT,
                          CW_USEDEFAULT,
                          MAIN_WINDOW_WIDTH,
                          MAIN_WINDOW_HEIGHT,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hwnd, nCmdShow);

    /* enable OpenGL for the window */
    EnableOpenGL(hwnd, &hDC, &hRC);

    /* program main loop */
    while (!bQuit)
    {
        /* check for messages */
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            /* handle or dispatch messages */
            if (msg.message == WM_QUIT)
            {
                bQuit = TRUE;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            /* OpenGL animation code goes here */

            if (menu_choice && sub_menu_choice && difficult_choice && side_choice && flash_done)
            {
                glClearColor(CHESSBOARD_R, CHESSBOARD_G, CHESSBOARD_B, 0.0f);
                glClear(GL_COLOR_BUFFER_BIT);

                glPushMatrix();
                glRotatef(theta, 0.0f, 0.0f, 0.0f);

//                glBegin(GL_TRIANGLES);
//                {
//                    glColor3f(1.0f, 0.0f, 0.0f);   glVertex2f(0.0f,   1.0f);
//                    glColor3f(0.0f, 1.0f, 0.0f);   glVertex2f(0.87f,  -0.5f);
//                    glColor3f(0.0f, 0.0f, 1.0f);   glVertex2f(-0.87f, -0.5f);
//                }
//                glEnd();

                draw_interface();

                if (turn == -human_side && !game_abort)
                {
                    if (!(computer_depth%2))
                    {
                        computer_chess(-human_side, computer_depth, computer_defense);
                    }
                    else
                    {
                        computer_chess(human_side, computer_depth, computer_defense);
                    }

                    //computer_chess(-human_side, computer_depth, computer_defense);
                }

                glPopMatrix();

                SwapBuffers(hDC);

                //theta += 1.0f;
                //Sleep (1);
            }
            else
            {
                glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
                glClear(GL_COLOR_BUFFER_BIT);

                glPushMatrix();
                glRotatef(theta, 0.0f, 0.0f, 0.0f);

                draw_menu();
                //draw_interface();   // for test

                glPopMatrix();

                SwapBuffers(hDC);
            }

        }

        if (!game_abort)
        {
            if (win_side)
            {
                if (win_side == WHITE_CHESS)
                {
                    wsprintf(szBuffer,  "白方获胜！");

                }
                else if (win_side == BLACK_CHESS)
                {
                    wsprintf(szBuffer,  "黑方获胜！");
                }
                else if (win_side == TIE)
                {
                    wsprintf(szBuffer,  "五子棋也能平局，水平相当！");
                }
                MessageBox(hwnd, szBuffer, TEXT("游戏结束"), MB_OK);
                game_abort = 1;
            }
        }

    }

    /* shutdown OpenGL */
    DisableOpenGL(hwnd, hDC, hRC);

    /* destroy the window explicitly */
    DestroyWindow(hwnd);

    return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    TCHAR szBuffer[128];    // Temp variety, can be deleted later
    static int x, y, down_x, down_y, down_flag;

    x = LOWORD(lParam);
    y = HIWORD(lParam);
    button_down = check_button(x, y);

    if (down_flag)
    {
        chessable_judgement(down_x, down_y);
    }
    else
    {
        chessable_judgement(x, y);
    }

    switch (uMsg)
    {
        /*case WM_LBUTTONDBLCLK:
            if (!game_abort)
            {
                brain_chess_deactivate();
            }
            break;
        why does it make no difference??? */

        case WM_LBUTTONDOWN:
            down_x = x;
            down_y = y;
            down_flag = 1;
            break;

        case WM_RBUTTONDOWN:
            down_x = x;
            down_y = y;
            down_flag = 1;
            break;

        case WM_LBUTTONUP:
            //x = LOWORD(lParam);
            //y = HIWORD(lParam);
            down_flag = 0;

            if (menu_choice && sub_menu_choice && difficult_choice && side_choice && flash_done)
            {
                return_to_menu_judgement(down_x, down_y, x, y);
                // there can be a choice about whether it is allowed to quit game with a game undone

                if (!game_abort)
                {
                    if (brain_turn_number == turn_number)
                    {
                        chess_judgement(down_x, down_y, x, y, REAL_CHESS);
                        undo_judgement(down_x, down_y, x, y);
                    }
                    else
                    {
                        brain_chess_undo();
                    }
                }
                else
                {
                    restart_judgement(down_x, down_y, x, y);
                    //return_to_menu_judgement(down_x, down_y, x, y);
                }
            }
            else
            {
                if (!flash_done)
                {
                    menu_button_down_judgement(down_x, down_y, x, y);
                }
            }

            //wsprintf(szBuffer,  "鼠标左键被单击, 击中位置: (%i, %i)", x, y);
            //MessageBox(hwnd, szBuffer, TEXT("鼠标动作"), MB_OK);
            //return 0;
            break;

        case WM_MBUTTONUP:
            if (!game_abort)
            {
                brain_chess_deactivate();
            }
            //wsprintf(szBuffer,  "鼠标中键被单击, 击中位置: (%i, %i)", x, y);
            //MessageBox(hwnd, szBuffer, TEXT("鼠标动作"), MB_OK);
            //return 0;
            break;

        case WM_RBUTTONUP:
            down_flag = 0;
            if (!game_abort)
            {
                chess_judgement(down_x, down_y, x, y, BRAIN_CHESS);
            }
            //wsprintf(szBuffer,  "鼠标右键被单击, 击中位置: (%i, %i)", x, y);
            //MessageBox(hwnd, szBuffer, TEXT("鼠标动作"), MB_OK);
            //return 0;
            break;

        case WM_CLOSE:
            PostQuitMessage(0);
            break;

        case WM_DESTROY:
            return 0;

        case WM_KEYDOWN:
            {
                switch (wParam)
                {
                    case VK_ESCAPE:
                        PostQuitMessage(0);
                    break;
                }
            }
            break;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

void EnableOpenGL(HWND hwnd, HDC* hDC, HGLRC* hRC)
{
    PIXELFORMATDESCRIPTOR pfd;

    int iFormat;

    /* get the device context (DC) */
    *hDC = GetDC(hwnd);

    /* set the pixel format for the DC */
    ZeroMemory(&pfd, sizeof(pfd));

    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;

    iFormat = ChoosePixelFormat(*hDC, &pfd);

    SetPixelFormat(*hDC, iFormat, &pfd);

    /* create and enable the render context (RC) */
    *hRC = wglCreateContext(*hDC);

    wglMakeCurrent(*hDC, *hRC);
}

void DisableOpenGL (HWND hwnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hwnd, hDC);
}

