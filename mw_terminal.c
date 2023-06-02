#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include "./linkedList/linkedList.h"

// Pixel content
#define EMPTY 0
#define SECTION 1
#define BORDER 2

// Pixel status
#define ACTIVE 1
#define INACTIVE 0
#define FOCUSED 1
#define UNFOCUSED 0

// Section Names
#define LIST 0
#define LABEL 1
#define TEXT_BOX 2

// Section types
#define STATIC 0
#define EDITABLE 1

#define MIN(a, b) ((a) < (b) ? (a) : (b))

char *DATA_TYPE = "str";

typedef int pixel_type;
typedef int section_name;
typedef int section_type;
typedef struct Section Section;

typedef struct Widget
{
    size_t width;
    size_t height;
    size_t pos_x;
    size_t pos_y;
    size_t id;
    LinkedList *sections;
} Widget;

struct Section
{
    void *el;
    section_name id;
    section_type type;
    size_t width;
    size_t height;
    size_t pos_x;
    size_t pos_y;
};

typedef struct List
{
    LinkedList *items;
    size_t min_width;
    size_t item_count;
} List;

typedef struct TextBox
{
    char *content;
} TextBox;

typedef struct Label
{
    char *content;
} Label;

typedef struct Pixel
{
    char pixel;
    bool active;
    bool focused;
    pixel_type type;
} Pixel;

void hideCursor()
{
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    cursorInfo.dwSize = 100;
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(consoleHandle, &cursorInfo);
}

void getWindowSize(int *width, int *height)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    *width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    *height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    // *width = csbi.dwMaximumWindowSize.X;
    // *height = csbi.dwMaximumWindowSize.Y;
}

void exit_on_error(char *message, const char *func, int line)
{
    printf("Error: %s on \"%s\" function in line %d\n", message, func, line);
    exit(-1);
}

Pixel allocate_pixel(char pixel, pixel_type type, bool active, bool focused)
{
    Pixel p;
    p.pixel = pixel;
    p.type = type;
    p.active = active;
    p.focused = focused;

    return p;
}

void initialize2DArray(Pixel ***arrax, int width, int height)
{
    *arrax = (Pixel **)malloc(height * sizeof(Pixel *)); // Allocate memorx for rows

    if (*arrax == NULL)
    {
        printf("Memorx allocation failed for arrax.\n");
        return;
    }
    //
    int i;
    for (i = 0; i < height; i++)
    {
        (*arrax)[i] = (Pixel *)malloc(width * sizeof(Pixel)); // Allocate memorx for columns of each row
        if ((*arrax)[i] == NULL)
        {
            printf("Memorx allocation failed for arrax elemnt at %i.\n", i);
            return;
        }
    }

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {

            (*arrax)[y][x] = allocate_pixel('-', EMPTY, INACTIVE, UNFOCUSED);
        }
    }
}

void create_window(Pixel ***pixels, int *width, int *height)
{
    getWindowSize(width, height);
    initialize2DArray(pixels, *width, *height);
}

Widget *allocate_widget(size_t width, size_t height,
                        size_t pos_x, size_t pos_y,
                        size_t id, LinkedList *sections)
{
    Widget *w = malloc(sizeof(Widget));

    w->width = width;
    w->height = height;
    w->pos_x = pos_x;
    w->pos_y = pos_y;
    w->sections = sections;
    w->id = id;

    return w;
}

Section *allocate_section(void *el, section_name name, size_t width, size_t heigth, size_t pos_x, size_t pos_y)
{
    Section *s = malloc(sizeof(Section));
    s->el = el;
    s->type = name;
    s->pos_x = pos_x;
    s->pos_y = pos_y;
    s->width = width;
    s->height = heigth;

    switch (name)
    {
    case LIST:
        s->type = STATIC;
        break;
    case LABEL:
        s->type = EDITABLE;
        break;
    case TEXT_BOX:
        s->type = EDITABLE;
        break;
    default:
        exit_on_error("Section type undefined", __func__, __LINE__);
        break;
    }

    return s;
}

List *allocate_list(LinkedList *items, size_t min_width)
{
    List *list = malloc(sizeof(List));

    list->items = items;
    list->min_width = min_width;
    list->item_count = (size_t)len(items);

    return list;
}

Label *allocate_label(char *title)
{
    Label *label = malloc(sizeof(Label));
    if (label != NULL)
    {
        label->content = malloc(strlen(title) + 1);

        if (label->content != NULL)
        {
            strcpy(label->content, title);
        }
        else
        {
            free(label);
            return NULL;
        }
    }
    return label;
}

size_t calculate_min_width(LinkedList *list)
{
    size_t min_width = 0;
    char **values = malloc(len(list) * sizeof(char *));
    for (int i = 0; i < len(list); ++i)
    {
        values[i] = (char *)get(list, i);
        if (strlen(values[i]) > min_width)
        {
            min_width = strlen(values[i]);
        }
    }
    return min_width;
}

void update_section(Pixel ***pixels, Widget *w)
{
    if (w->sections == NULL)
    {
        return;
    }
    LinkedList *sections = w->sections;
    Section *s;

    if (sections == NULL)
    {
        return;
    }

    bool completed = false;

    while (sections != NULL)
    {
        s = sections->value;
        //     printf("s width: %zu, s height: %zu, s pos_x: %zu, s pos_y: %zu\n",
        //            s->width, s->height, s->pos_x, s->pos_y);
        //     printf("w width: %zu, w height: %zu, w pos_x: %zu, w pos_y: %zu\n",
        //            w->width, w->height, w->pos_x, w->pos_y);

        size_t y_start = w->pos_y + s->pos_y + 1;
        size_t x_start = w->pos_x + s->pos_x + 1;
        size_t y_limit = MIN((size_t)abs(s->height - y_start), (size_t)abs(w->height - y_start) + 1) + y_start;
        size_t x_limit = MIN((size_t)abs(s->width - x_start), (size_t)abs(w->width - x_start)) + x_start;

        // printf("x_start: %zu, y_start: %zu, x_limit: %zu, y_limit: %zu\n",
        //        x_start, y_start, x_limit, y_limit);

        for (size_t y = y_start; y < y_limit; ++y)
        {
            if (completed)
            {
                break;
            }
            for (size_t x = x_start; x < x_limit; ++x)
            {
                if (s->type == LIST)
                {
                    List *list = s->el;
                    LinkedList *items_list = list->items;

                    char *item;
                    while (items_list != NULL)
                    {
                        item = items_list->value;
                        size_t item_length = strlen(item);
                        for (size_t i = 0; i < item_length; ++i)
                        {
                            Pixel *temp = &((*pixels)[y][x]);

                            temp->pixel = item[i];
                            temp->active = ACTIVE;
                            temp->focused = UNFOCUSED;
                            temp->type = SECTION;
                            x++;
                        }

                        items_list = items_list->next;
                        y++;
                        x = x - item_length;
                    }
                    completed = true;
                    break;
                }
                else if (s->type == LABEL)
                {
                    Label *label = s->el;
                    char *content = label->content;

                    for (size_t i = 0; i < strlen(content); ++i)
                    {
                        Pixel *temp = &((*pixels)[y][x]);

                        temp->pixel = content[i];
                        temp->active = ACTIVE;
                        temp->focused = UNFOCUSED;
                        temp->type = SECTION;
                        x++;
                    }
                    completed = true;
                    break;
                }
                else
                {
                    exit_on_error("Section type undefined", __func__, __LINE__);
                    break;
                }
            }
        }

        sections = sections->next;
        completed = false;
    }
}

void update_pixels(Pixel ***pixels, LinkedList *widgets, size_t window_width, size_t window_height)
{

    (void)window_height;

    LinkedList *current = widgets;
    Widget *el;

    while (current != NULL)
    {
        el = current->value;

        // printf("%d\n", current == NULL);
        // printf("width: %zu, height: %zu, pos_x: %zu, pos_y: %zu\n",
        //        el->width, el->height, el->pos_x, el->pos_y);

        update_section(pixels, el);

        size_t y_start = el->pos_y;
        size_t x_start = el->pos_x;
        size_t y_limit = el->height + el->pos_y;
        size_t x_limit = el->width + el->pos_x;

        if (window_width % 2 != 0)
        {
            x_limit++;
        }

        if (window_height % 2 != 0)
        {
            y_limit++;
        }

        // printf("x_limit: %zu, y_limit: %zu\n", x_limit, y_limit);
        for (size_t y = y_start; y < y_limit; ++y)
        {
            for (size_t x = x_start; x < x_limit; ++x)
            {

                Pixel *temp = &((*pixels)[y][x]);
                // printf("%d ", temp->type);
                if (!temp->active)
                {
                    if (y == 0 || y == y_limit - 1)
                    {
                        temp->pixel = '*';
                        temp->active = ACTIVE;
                        temp->focused = UNFOCUSED;
                        temp->type = BORDER;
                    }
                    else if (x == x_start || x == x_limit - 1)
                    {
                        temp->pixel = '*';
                        temp->active = ACTIVE;
                        temp->focused = UNFOCUSED;
                        temp->type = BORDER;
                    }
                    else
                    {
                        temp->pixel = ' ';
                        temp->active = INACTIVE;
                        temp->focused = UNFOCUSED;
                        temp->type = EMPTY;
                    }
                }
            }
        }
        current = current->next;
    }
}

void clean_element(Pixel ***pixels, Widget *curr_widget, Section *curr_section, int focus_index)
{
    size_t y_clean_start = 0;
    size_t y_clean_end = 0;
    size_t x_clean_start = 0;
    size_t x_clean_end = 0;

    if (curr_section->type == LIST)
    {
        List *list = curr_section->el;
        y_clean_start = curr_widget->pos_y + curr_section->pos_y + focus_index;
        y_clean_end = y_clean_start + 1;
        x_clean_start = curr_widget->pos_x + curr_section->pos_x;
        x_clean_end = x_clean_start + list->min_width;
    }
    else
    {
        exit_on_error("Section type undefined for new_section", __func__, __LINE__);
    }

    // Clean section pixels
    for (size_t y = y_clean_start; y < y_clean_end; y++)
    {

        for (size_t x = x_clean_start; x < x_clean_end; ++x)
        {
            ((*pixels)[y][x]).focused = UNFOCUSED;
        }
    }
}

void next_element(Pixel ***pixels, Section *curr_section, Widget *curr_widget,
                  int focus_index)
{

    // printf("pos_x: %zu, pos_y: %zu, focus_index: %zu\n",
    //        curr_section->pos_x, curr_section->pos_y,
    //        focus_index);

    if (focus_index - 1 >= 0)
    {
        // Not first element, can clean previous element
        clean_element(pixels, curr_widget, curr_section, focus_index - 1);
    }

    size_t y_new_start;
    size_t y_new_end;
    size_t x_new_start;
    size_t x_new_end;

    if (curr_section->type == LIST)
    {
        List *list = curr_section->el;
        y_new_start = curr_widget->pos_y + curr_section->pos_y + focus_index + 1;
        y_new_end = y_new_start + 1;
        x_new_start = curr_widget->pos_x + curr_section->pos_x + 1;
        x_new_end = x_new_start + list->min_width;
    }
    else
    {
        exit_on_error("Section type undefined for new_section", __func__, __LINE__);
    }

    // Clean previous focused pixels
    //     {
    //         size_t y = s->pos_y + focus_index;
    //         for (size_t y = 0; y < count; y++)
    //         {

    //             for (size_t x = s->pos_x; x < s->pos_x + s->min_width; ++x)
    //             {
    //                 ((*pixels)[y][x]).active = INACTIVE;
    //             }
    //         }
    //     }

    // Set new focused pixels
    for (size_t y = y_new_start; y < y_new_end; y++)
    {

        for (size_t x = x_new_start; x < x_new_end; ++x)
        {
            ((*pixels)[y][x]).focused = FOCUSED;
        }
    }
}
// void prev_element(Pixel ***pixels, Widget *w, int element_index)
// {

//     // printf("pos_x: %zu, pos_y: %zu, element_index: %zu, item_count: %zu, min_width: %zu\n",
//     //        w->list->pos_x, w->list->pos_y,
//     //        element_index, w->list->item_count,
//     //        w->list->min_width);

//     // Set next element to inactive
//     {
//         size_t y = w->list->pos_y + element_index;
//         for (size_t x = w->list->pos_x; x < w->list->pos_x + w->list->min_width; ++x)
//         {
//             ((*pixels)[y][x]).active = INACTIVE;
//         }
//     }

//     // Set previous element to active
//     {
//         size_t y = w->list->pos_y + (element_index - 1);
//         for (size_t x = w->list->pos_x; x < w->list->pos_x + w->list->min_width; ++x)
//         {
//             ((*pixels)[y][x]).active = ACTIVE;
//         }
//     }
// }

// void next_widget(Pixel ***pixels, Widget *prev_widget, Widget *active_widget, int element_index)
// {
//     // Clear element from previous widget
//     {
//         size_t y = prev_widget->list->pos_y + element_index;
//         for (size_t x = prev_widget->list->pos_x;
//              x < prev_widget->list->pos_x + prev_widget->list->min_width;
//              ++x)
//         {
//             ((*pixels)[y][x]).active = INACTIVE;
//         }
//     }

//     // Active first item of new widget
//     {
//         size_t y = active_widget->list->pos_y;
//         for (size_t x = active_widget->list->pos_x; x < active_widget->list->pos_x + active_widget->list->min_width;
//              ++x)
//         {
//             ((*pixels)[y][x]).active = ACTIVE;
//         }
//     }
// }

void render(Pixel **pixels, size_t window_width, size_t window_height)
{
    for (size_t y = 0; y < (size_t)window_height; y++)
    {
        for (size_t x = 0; x < (size_t)window_width; x++)
        {
            Pixel curr_pixel = pixels[y][x];
            
            printf("%c", curr_pixel.pixel);

            if (curr_pixel.focused)
            {
                HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
                COORD coord;
                coord.X = x; // Starting X position
                coord.Y = y; // Starting Y position
                WORD attribute = 0 | BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE;
                DWORD written;

                FillConsoleOutputAttribute(hConsole, attribute, 1, coord, &written);
            }


        }
    }
}

int main(void)
{
    // Lists creation
    char **todo_items;
    size_t todo_items_len = 3;
    todo_items = (char **)malloc(todo_items_len * sizeof(char *));
    todo_items[0] = createStr("- [ ] Buy a bread");
    todo_items[1] = createStr("- [ ] Eat");
    todo_items[2] = createStr("- [ ] Drink code");

    LinkedList *todo = convertArray(todo_items, todo_items_len);
    size_t todo_min_width = calculate_min_width(todo);

    char **done_items;
    size_t done_items_len = 3;
    done_items = (char **)malloc(done_items_len * sizeof(char *));
    done_items[0] = createStr("- [x] sleep");
    done_items[1] = createStr("- [x] high");
    done_items[2] = createStr("- [x] low");

    LinkedList *done = convertArray(done_items, done_items_len);
    size_t done_min_width = calculate_min_width(done);

    // Create window
    int window_width, window_height;
    Pixel **pixels;
    create_window(&pixels, &window_width, &window_height);
    // printf("Window size: %d window_width x %d window_height\n", window_width, window_height);

    size_t widget_width = (size_t)window_width / 2;
    size_t widget_height = (size_t)window_height / 2;

    // Create todo widget
    List *todo_list = allocate_list(todo, todo_min_width);
    Section *todo_section = allocate_section(todo_list, LIST, 0, 0, (widget_width / 2 - todo_min_width / 2), 1);
    Label *todo_label = allocate_label("TODO");
    Section *todo_label_section = allocate_section(todo_label, LABEL, 0, 0, ((widget_width - 1) / 2 - 5 / 2) - 1, 0);
    LinkedList *todo_widget_sections = add(NULL, todo_label_section);
    add(todo_widget_sections, todo_section);

    // Create done widget
    List *done_list = allocate_list(done, done_min_width);
    Section *done_section = allocate_section(done_list, LIST, 0, 0, (widget_width / 2 - done_min_width / 2), 1);
    Label *done_label = allocate_label("DONE");
    Section *done_label_section = allocate_section(done_label, LABEL, 0, 0, ((widget_width - 1) / 2 - 5 / 2) - 1, 0);
    LinkedList *done_widget_sections = add(NULL, done_label_section);
    add(done_widget_sections, done_section);

    Widget *w = allocate_widget(widget_width, widget_height,
                                0, 0,
                                0, todo_widget_sections);
    Widget *w2 = allocate_widget(widget_width, widget_height,
                                 widget_width, 0,
                                 1, done_widget_sections);
    Widget *w3 = allocate_widget(widget_width, widget_height,
                                 0, widget_height,
                                 2, NULL);
    Widget *w4 = allocate_widget(widget_width, widget_height,
                                 widget_width, widget_height,
                                 3, NULL);

    LinkedList *widgets = add(NULL, w);

    add(widgets, w2);
    add(widgets, w3);
    add(widgets, w4);
    // add_widgets(&pixels, widgets);
    update_pixels(&pixels, widgets, window_width, window_height);
    system("cls");
    hideCursor();

    int widget_id = 0;
    Widget *curr_widget = get(widgets, widget_id);
    LinkedList *curr_widget_sections = curr_widget->sections;
    Section *curr_section = curr_widget_sections->next->value;

    int focus_index = 0;
    next_element(&pixels, curr_section, curr_widget, focus_index);

    render(pixels, window_width, window_height);

    // char key;
    // int arrowCode;
    // bool need_to_render = true;

    // while (key != 'q')
    // {
    //     if (need_to_render)
    //     {
    //         system("cls");

    //         render(pixels, window_width, window_height);
    //         need_to_render = false;
    //     }

    //     key = _getch();

    //     switch (key)
    //     {
    //     case -32:
    //         arrowCode = _getch();

    //         if (arrowCode == 80)
    //         {
    //             // Arrow down
    //             if (curr_section->type == LIST)
    //             {
    //                 List *list = curr_section->el;
    //                 if ((size_t)(focus_index) + 1 < list->item_count)
    //                 {
    //                     next_element(&pixels, curr_widget_sections, curr_widget, focus_index);
    //                     focus_index++;
    //                     need_to_render = true;
    //                 }
    //             }
    //             else
    //             {
    //                 exit_on_error("Section type undefined", __func__, __LINE__);
    //             }
    //         }
    //         // else if (arrowCode == 72)
    //         // {
    //         //     // Arrow up
    //         //     if (focus_index > 0)
    //         //     {
    //         //         prev_element(&pixels, curr_widget, focus_index);
    //         //         focus_index--;
    //         //         need_to_render = true;
    //         //     }
    //         // }
    //         // break;
    //     // case 9:
    //     //     // Tab
    //     //     if (GetKeyState(VK_SHIFT) & 0x8000)
    //     //     {
    //     //         //  Shift + Tab
    //     //         if (widget_id >= 0)
    //     //         {
    //     //             widget_id++;
    //     //             Widget *prev = curr_widget;
    //     //             curr_widget = get(widgets, widget_id);
    //     //             if (curr_widget->list != NULL)
    //     //             {
    //     //                 next_widget(&pixels, prev, curr_widget, element_index);
    //     //                 element_index = 0;
    //     //                 need_to_render = true;
    //     //             }
    //     //             else
    //     //             {
    //     //                 curr_widget = prev;
    //     //                 widget_id--;
    //     //             }
    //     //         }
    //     //         break;
    //     //     }

    //     //     if (widget_id + 1 < len(widgets))
    //     //     {
    //     //         widget_id++;
    //     //         Widget *prev = curr_widget;
    //     //         curr_widget = get(widgets, widget_id);
    //     //         if (curr_widget->list != NULL)
    //     //         {
    //     //             next_widget(&pixels, prev, curr_widget, element_index);
    //     //             element_index = 0;
    //     //             need_to_render = true;
    //     //         }
    //     //         else
    //     //         {
    //     //             curr_widget = prev;
    //     //             widget_id--;
    //     //         }
    //     //     }
    //     //     break;
    //     default:
    //         break;
    //     }
    // }

    // Free the dynamically allocated memory

    for (size_t i = 0; i < (size_t)window_height; i++)
    {
        free(pixels[i]);
    }
    free(pixels);

    for (size_t i = 0; i < todo_items_len; i++)
    {
        free(todo_items[i]);
    }
    free(todo_items);

    for (size_t i = 0; i < done_items_len; i++)
    {
        free(done_items[i]);
    }
    free(done_items);

    freeList(todo);
    freeList(done);

    return 0;
}