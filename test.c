#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <stdbool.h>

// void hideCursor()
// {
//     HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
//     CONSOLE_CURSOR_INFO cursorInfo;
//     cursorInfo.dwSize = 100;
//     cursorInfo.bVisible = FALSE;
//     SetConsoleCursorInfo(consoleHandle, &cursorInfo);
// }

// void getWindowSize(int *columns, int *rows)
// {
//     CONSOLE_SCREEN_BUFFER_INFO csbi;
//     GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
//     *columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
//     *rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
// }

// void getWindowSizeInPixels(int *width, int *height)
// {
//     RECT rect;
//     HWND consoleWindow = GetConsoleWindow();
//     GetWindowRect(consoleWindow, &rect);
//     *width = rect.right - rect.left;
//     *height = rect.bottom - rect.top;
// }

// void setColor(int foreground, int background)
// {
//     HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
//     int color = foreground + (background * 16);
//     SetConsoleTextAttribute(consoleHandle, color);
// }

// void setColor2(int attributes)
// {
//     HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
//     SetConsoleTextAttribute(consoleHandle, attributes);
// }

// void getCurrentColors(WORD *foreground, WORD *background)
// {
//     HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
//     CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
//     GetConsoleScreenBufferInfo(consoleHandle, &consoleInfo);
//     *foreground = consoleInfo.wAttributes & 0x0F;
//     *background = (consoleInfo.wAttributes & 0xF0) >> 4;
// }

// void drawWidget(int rows, int columns)
// {
//     char *space = malloc((columns - 2) * sizeof(char));

//     if (space == NULL)
//     {
//         printf("Memory allocation failed.\n");
//         return;
//     }

//     for (int k = 0; k < columns - 2; k++)
//     {
//         space[k] = ' ';
//     }

//     space[columns - 2] = '\0';

//     for (int i = 0; i < rows; i++)
//     {
//         if (i == 0 || i == rows - 1)
//         {
//             for (int j = 0; j < columns; j++)
//             {
//                 printf("-");
//             }
//         }
//         else
//         {
//             printf("|%s|", space);
//         }
//     }
// }

// void add_widgets(char ***pixels, LinkedList *widgets)
// {
//     size_t item_index = 0;
//     bool typed_item = false;
//     for (int i = 0; i < len(widgets); ++i)
//     {
//         item_index = 0;
//         Widget *w = get(widgets, i);
//         // printf("width: %d, height: %d, x_off_set: %d, y_off_set: %d, has list: %d\n",
//         //        w->width, w->height,
//         //        w->x_off_set, w->y_off_set, w->list != NULL);
//         for (size_t y = w->y_off_set; y < (w->height + w->y_off_set); y++)
//         {
//             typed_item = false;
//             for (size_t x = w->x_off_set; x < (w->width + w->x_off_set); x++)
//             {
//                 // printf("y: %d, ", y);
//                 // printf("x: %d\n", x);
//                 if (y == 0 || y == (w->height + w->y_off_set) - 1)
//                 {
//                     // print horizontal borders
//                     (*pixels)[y][x] = '*';
//                 }
//                 else if (x == 0 || x == (w->width + w->x_off_set) - 1)
//                 {
//                     // print vertical borders
//                     (*pixels)[y][x] = '*';
//                 }
//                 else if (y == 1 && strlen(w->title) > 0)
//                 {
//                     // Print widgte name if contains
//                     if (x == ((w->width / 2) - strlen(w->title) / 2) + w->x_off_set)
//                     {
//                         for (size_t i = 0; i < strlen(w->title); ++i)
//                         {
//                             // printf("%s\n", values[list_index]);
//                             if (w->title[i] != '\0')
//                             {
//                                 (*pixels)[y][x++] = w->title[i];
//                             }
//                         }
//                         x--;
//                     }
//                     else
//                     {
//                         (*pixels)[y][x] = ' ';
//                     }
//                 }
//                 else if (!typed_item && w->list != NULL &&
//                          y >= w->list->pos_y &&
//                          y < (w->list->pos_y + w->list->item_count) &&
//                          x == w->list->pos_x)
//                 {
//                     // Print current list
//                     List *curr_list = w->list;
//                     char *curr_item = (char *)get(curr_list->items, item_index);
//                     size_t item_len = strlen(curr_item);

//                     for (size_t j = 0; j < item_len; ++j)
//                     {
//                         // printf("%s\n", values[list_index]);
//                         (*pixels)[y][x++] = curr_item[j];
//                     }
//                     typed_item = true;

//                     x--;
//                     item_index++;
//                 }
//                 else
//                 {
//                     // Add blank spaces
//                     (*pixels)[y][x] = ' ';
//                 }
//             }
//         }
//     }
// }

int main(void)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coord;
    coord.X = 1; // Starting X position
    coord.Y = 1; // Starting Y position
    WORD attribute = FOREGROUND_RED;
    DWORD written;

    FillConsoleOutputAttribute(hConsole, attribute, 1, coord, &written);
    // WORD foreground, background;
    // getCurrentColors(&foreground, &background);

    // printf("Current foreground color: %d\n", foreground);
    // printf("Current background color: %d\n", background);

    // int columns = 1;
    // int rows;
    // getWindowSize(&columns, &rows);
    // printf("Window size: %d columns x %d rows\n", columns, rows);

    // system("cls");
    // drawWidget(rows, columns / 2);
    //     // if (j == 0 || j == columns - 1)
    //     // {
    //     //     printf("-");
    //     // }
    //     // printf("\n");
    //     printf("-\n");
    // int width, height;
    // getWindowSizeInPixels(&width, &height);

    // printf("Window size: %d pixels (width) x %d pixels (height)\n", width, height);

    // setColor2(0 | BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE);
    // printf("This sentence has black text on a white background.\n");

    // getCurrentColors(&foreground, &background);

    // printf("Current foreground color: %d\n", foreground);
    // printf("Current background color: %d\n", background);

    // setColor2(15);
    // printf("back to normal.\n");

    // getCurrentColors(&foreground, &background);

    // printf("Current foreground color: %d\n", foreground);
    // printf("Current background color: %d\n", background);

    // setColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY, BACKGROUND_GREEN | BACKGROUND_RED);
    // printf("This sentence has bright green text and a yellow background.\n");

    // setColor(FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED, BACKGROUND_INTENSITY);
    // printf("This sentence has white text and an intense background color.\n");

    // hideCursor();
    // char str[20];

    // printf("Enter a number: ");

    // printf("You entered: %s\n", str);
    // system("echo off");

    // while (1)
    // {
    //     system("cls");
    //     scanf("%s", str);
    //     printf("%s\n", str);
    //     fflush(stdout);
    //     Sleep(100);
    // }
    // while (1)
    // {
    //     if ((GetKeyState(VK_SHIFT) & 0x8000) && (GetKeyState(VK_MENU) & 0x8000))
    //     {
    //         printf("Shift + Alt pressed\n");
    //         break;
    //     }
    // }
    // while (1)
    // {
    //     if ()
    //     {
    //         printf("Shift + Alt pressed\n");
    //         break;
    //     }
    // }
    // char ch;
    // int arrowCode;

    // while ((ch = _getch()) != '\r')
    // {
    //     printf("%c\n", ch);
    //     switch (ch)
    //     {
    //     case -32:
    //         arrowCode = _getch();

    //         if (arrowCode == 80)
    //         {
    //             printf("Arrow down key was pressed\n");
    //         }
    //         else if (arrowCode == 72)
    //         {
    //             printf("Arrow up key was pressed\n");
    //         }

    //         break;
    //     case '\t':
    //         if(GetKeyState(VK_SHIFT) & 0x8000)
    //         {
    //             printf("shift tab\n");
    //         }
    //         break;
    //     default:
    //         break;
    //     }

    // }
    // HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    // COORD coord;
    // coord.X = 0; // Starting X position
    // coord.Y = 0; // Starting Y position
    // DWORD numCellsToFill = 2000;
    // WORD attribute = FOREGROUND_RED | FOREGROUND_INTENSITY | BACKGROUND_BLUE;
    // DWORD numberOfCharsWritten;

    // FillConsoleOutputAttribute(hConsole, attribute, numCellsToFill, coord, &numberOfCharsWritten);
    // HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    // COORD coord = {0, 0};
    // DWORD numberOfCharsWritten;

    // FillConsoleOutputCharacter(hConsole, 'X', 10, coord, &numberOfCharsWritten);

    // printf("%lu\n", numberOfCharsWritten);
    // HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    // if (hConsole == INVALID_HANDLE_VALUE)
    // {
    //     // Handle is invalid
    //     printf("Invalid handle\n");
    //     return 1; // Exit the program or handle the error accordingly
    // }
    // COORD coord;
    // coord.X = 0; // Starting X position
    // coord.Y = 0; // Starting Y position
    // DWORD numCellsToFill = 1;
    // WORD attribute = FOREGROUND_RED | FOREGROUND_INTENSITY | BACKGROUND_BLUE;
    // bool status = FillConsoleOutputAttribute(hConsole, attribute, numCellsToFill, coord, NULL);
    // printf("%d\n", status);

    // size_t x = 0;
    // size_t y = 0;

    // HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    // COORD coord;
    // coord.X = x; // Starting X position
    // coord.Y = y; // Starting Y position
    // WORD attribute = 0 | BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE;
    // DWORD written;

    // FillConsoleOutputAttribute(hConsole, attribute, 1, coord, &written);
    // CONSOLE_SCREEN_BUFFER_INFO csbi;
    // GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    // printf("Size x: %d, Size y: %d\n", csbi.dwSize.X, csbi.dwSize.Y);
    // printf("Cursor x: %d, Cursor y: %d\n", csbi.dwCursorPosition.X, csbi.dwCursorPosition.Y);
    // printf("Maximun x: %d, Maximun y: %d\n", csbi.dwMaximumWindowSize.X, csbi.dwMaximumWindowSize.Y);
    // printf("Attributes: %d\n", csbi.wAttributes);
    // printf("Rect left: %d, Rect top: %d, Rect right: %d, Rect bottom: %d\n", csbi.srWindow.Left, csbi.srWindow.Top,
    //         csbi.srWindow.Right, csbi.srWindow.Bottom);
    // size_t x = 59 + 59;
    // printf("%zu\n", x);
    return 0;
}

// void add_widgets(Pixel ***pixels, LinkedList *widgets)
// {
//     size_t item_index = 0;
//     bool typed_item = false;
//     char empty = ' ';

//     for (int i = 0; i < len(widgets); ++i)
//     {
//         item_index = 0;
//         Widget *w = get(widgets, i);
//         // printf("width: %d, height: %d, pos_x: %d, pos_y: %d, has list: %d\n",
//         //        w->width, w->height,
//         //        w->pos_x, w->pos_y, w->list != NULL);
//         for (size_t y = w->pos_y; y < (w->height + w->pos_y); y++)
//         {
//             typed_item = false;
//             for (size_t x = w->pos_x; x < (w->width + w->pos_x); x++)
//             {
//                 // printf("y: %d, ", y);
//                 // printf("x: %d\n", x);
//                 if (y == 0 || y == (w->height + w->pos_y) - 1)
//                 {
//                     // print horizontal borders
//                     (*pixels)[y][x] = allocate_pixel('*', BORDER, INACTIVE);
//                 }
//                 else if (x == 0 || x == (w->width + w->pos_x) - 1)
//                 {
//                     // print vertical borders
//                     (*pixels)[y][x] = allocate_pixel('*', BORDER, INACTIVE);
//                 }
//                 else if (y == 1 && strlen(w->title) > 0)
//                 {
//                     // Print widgte name if contains
//                     if (x == ((w->width / 2) - strlen(w->title) / 2) + w->pos_x)
//                     {
//                         for (size_t i = 0; i < strlen(w->title); ++i)
//                         {
//                             // printf("%s\n", values[list_index]);
//                             if (w->title[i] != '\0')
//                             {
//                                 (*pixels)[y][x++] = allocate_pixel(w->title[i], LABEL, INACTIVE);
//                             }
//                         }
//                         x--;
//                     }
//                     else
//                     {
//                         (*pixels)[y][x] = allocate_pixel(empty, EMPTY_SPACE, INACTIVE);
//                     }
//                 }
//                 else if (!typed_item && w->list != NULL &&
//                          y >= w->list->pos_y &&
//                          y < (w->list->pos_y + w->list->item_count) &&
//                          x == w->list->pos_x)
//                 {
//                     // Print current list
//                     List *curr_list = w->list;
//                     char *curr_item = (char *)get(curr_list->items, item_index);
//                     size_t item_len = strlen(curr_item);

//                     for (size_t j = 0; j < item_len; ++j)
//                     {
//                         // printf("%s\n", values[list_index]);
//                         bool active = INACTIVE;
//                         if (item_index == 0 && w->id == 0)
//                         {
//                             active = ACTIVE;
//                         }
//                         (*pixels)[y][x++] = allocate_pixel(curr_item[j], ELEMENT, active);
//                     }
//                     typed_item = true;

//                     x--;
//                     item_index++;
//                 }
//                 else
//                 {
//                     // Add blank spaces
//                     (*pixels)[y][x] = allocate_pixel(empty, EMPTY_SPACE, INACTIVE);
//                 }
//             }
//         }
//     }
// }