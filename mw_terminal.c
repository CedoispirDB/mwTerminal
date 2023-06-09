#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include "./utils/linkedList.h"

#define EXIT_ON_ERROR(message)                                                            \
    do                                                                                    \
    {                                                                                     \
        printf("Error: %s on \"%s\" function in line %d\n", message, __func__, __LINE__); \
        exit(-1);                                                                         \
    } while (0)

#define EXIT_ON_ERROR_PAR(format, ...)                  \
    do                                                  \
    {                                                   \
        fprintf(stderr, "Error: " format, __VA_ARGS__); \
        exit(EXIT_FAILURE);                             \
    } while (0)

// Pixel content
#define EMPTY 0
#define SECTION 1
#define BORDER 2

// Pixel status
#define ACTIVE 1
#define INACTIVE 0
#define FOCUSED 1
#define UNFOCUSED 0

// Section type
#define LIST 0
#define LABEL 1
#define INPUT 2

// Section state
#define STATIC 0
#define EDITABLE 1

// Directions
#define PREVIOUS 0
#define NEXT 1
#define CURRENT 2
#define NONE 3

// List condition
#define CHECKED 1
#define UNCHECKED 0

// List type
#define CHECK_BOX 0

#define MIN(a, b) ((a) < (b) ? (a) : (b))

int DEBUG_STATE = 1;

char *DATA_TYPE = "str";

typedef int pixel_type;
typedef char *list_type;
typedef int section_state;
typedef int section_type;

typedef struct Window Window;
typedef struct Widget Widget;
typedef struct Section Section;
typedef struct Item Item;

typedef struct Pixel
{
    char pixel;
    bool active;
    bool focused;
    pixel_type type;
} Pixel;

struct Window
{
    const char *title;
    size_t id;
    size_t width;
    size_t height;
    Window *next;
    Window *prev;
    Pixel **pixels;
    Widget *widgets;
};

struct Widget
{
    char border_char;
    size_t border_size;
    size_t width;
    size_t height;
    size_t pos_x;
    size_t pos_y;
    size_t id;
    int current_section;
    Section *sections;
    Section *last_section;
    Widget *next;
    Widget *prev;
};

struct Section
{
    void *el;
    char *label;
    size_t id;
    size_t width;
    size_t height;
    size_t pos_x;
    size_t pos_y;
    section_state state;
    section_type type;
    Section *next;
    Section *prev;
};

typedef struct List
{
    Item *items;
    size_t item_count;
    Section *target;
    bool checked;
    list_type type;
} List;

typedef struct Label
{
    char *content;
} Label;

typedef struct Input
{
    int cursor_x;
    int cursor_y;
} Input;

struct Item
{
    char *text;
    Item *next;
    Item *prev;
};

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

Pixel alloc_pixel(char pixel, pixel_type type, bool active, bool focused)
{
    Pixel p;
    p.pixel = pixel;
    p.type = type;
    p.active = active;
    p.focused = focused;

    return p;
}

Pixel **initialize2DArray(size_t width, size_t height)
{
    Pixel **pixels;
    pixels = (Pixel **)malloc(height * sizeof(Pixel *)); // alloc memorx for rows

    if (pixels == NULL)
    {
        EXIT_ON_ERROR("Not able to allocate memory for pixels");
    }
    //
    size_t i;
    for (i = 0; i < height; i++)
    {
        (pixels)[i] = (Pixel *)malloc(width * sizeof(Pixel)); // alloc memorx for columns of each row
        if ((pixels)[i] == NULL)
        {
            EXIT_ON_ERROR("Not able to allocate memory for pixels");
        }
    }

    for (size_t y = 0; y < height; y++)
    {
        for (size_t x = 0; x < width; x++)
        {

            (pixels)[y][x] = alloc_pixel('-', EMPTY, INACTIVE, UNFOCUSED);
        }
    }

    return pixels;
}

Window create_window(const char *title, size_t id)
{
    int width, height;
    getWindowSize(&width, &height);

    Window w;
    w.title = title;
    w.id = id;
    w.width = width;
    w.height = height;
    w.next = NULL;
    w.prev = NULL;
    Pixel **pixels = initialize2DArray(width, height);
    w.pixels = pixels;
    w.widgets = NULL;

    return w;
}

Widget create_widget(char border_char, size_t border_size, size_t width, size_t height,
                     size_t pos_x, size_t pos_y,
                     size_t id, Section *sections)
{
    Widget w;

    w.border_char = border_char;
    w.border_size = border_size;
    w.width = width;
    w.height = height;
    w.pos_x = pos_x;
    w.pos_y = pos_y;
    w.id = id;
    w.sections = sections;
    w.current_section = -1;
    w.last_section = NULL;
    w.next = NULL;
    w.prev = NULL;

    return w;
}

Section create_section(void *el, section_type type, size_t width, size_t heigth, size_t pos_x, size_t pos_y, int id, char *label)
{
    Section s;
    s.el = el;
    s.label = label;
    s.id = id;
    s.width = width;
    s.height = heigth;
    s.pos_x = pos_x;
    s.pos_y = pos_y;
    s.next = NULL;
    s.prev = NULL;
    s.type = type;

    switch (type)
    {
    case LIST:
        s.state = EDITABLE;
        break;
    case LABEL:
        s.state = STATIC;
        break;
    case INPUT:
        s.state = EDITABLE;
        break;
    default:
        EXIT_ON_ERROR("Section type undefined");
        break;
    }

    return s;
}

List create_list(Item *items, list_type type, bool checked, Section *target)
{
    List list;
    list.items = items;
    list.item_count = 0;
    list.target = target;
    list.type = type;
    list.checked = checked;

    if (type == CHECK_BOX)
    {
        Item *temp = items;
        char *prefix;
        if (checked)
        {
            prefix = "- [x] ";
        }
        else
        {
            prefix = "- [ ] ";
        }
        while (temp != NULL)
        {
            char *value = temp->text;

            size_t prefixLength = strlen(prefix);
            size_t targetLength = strlen(value);

            size_t newLength = prefixLength + targetLength + 1;

            char *newString = malloc(newLength);

            strcpy(newString, prefix);

            strcat(newString, value);
            temp->text = newString;
            temp = temp->next;
        }
    }

    return list;
}

Label create_label(char *title)
{
    Label label;

    label.content = malloc(strlen(title) + 1);

    if (label.content != NULL)
    {
        strcpy(label.content, title);
    }
    else
    {
        EXIT_ON_ERROR("Not able to allocate memory for label title");
    }
    return label;
}

Input *create_input()
{
    Input *i = malloc(sizeof(Input));
    i->cursor_x = 0;
    i->cursor_y = 0;
    return i;
}

Item *create_items(char **array, size_t length)
{
    Item *head;
    Item *prev_item;

    for (size_t i = 0; i < length; ++i)
    {
        Item *new_item = malloc(sizeof(Item));
        new_item->text = malloc(strlen(array[i]) + 1);
        strcpy(new_item->text, array[i]);

        if (i == 0)
        {
            new_item->next = NULL;
            new_item->prev = NULL;
            prev_item = new_item;
            head = new_item;
            continue;
        }

        new_item->prev = prev_item;
        new_item->next = NULL;
        prev_item->next = new_item;
        prev_item = new_item;
    }

    return head;
}
void link_widgets(Widget *head, Widget *value)
{

    Widget *current = head;
    Widget *last;

    while (current != NULL)
    {
        last = current;
        current = current->next;
    }
    last->next = value;
    value->prev = last;
}

void link_sections(Section *head, Section *value)
{

    Section *current = head;
    Section *last;

    while (current != NULL)
    {
        last = current;
        current = current->next;
    }
    last->next = value;
    value->prev = last;
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



void update_sections(Pixel ***pixels, Widget *w)
{
    if (w->sections == NULL)
    {
        return;
    }
    Section *s = w->sections;

    if (s == NULL)
    {
        return;
    }

    bool completed = false;

    while (s != NULL)
    {
        printf("s width: %zu, s height: %zu, s pos_x: %zu, s pos_y: %zu\n",
               s->width, s->height, s->pos_x, s->pos_y);
        printf("w width: %zu, w height: %zu, w pos_x: %zu, w pos_y: %zu, border_size: %zu\n",
               w->width, w->height, w->pos_x, w->pos_y, w->border_size);
        // printf("Section type: %d\n", s->type);
        size_t y_start = w->pos_y + s->pos_y + w->border_size;
        size_t x_start = w->pos_x + s->pos_x + w->border_size;
        size_t y_limit = y_start + s->height;
        size_t x_limit = x_start + s->width;

        printf("x_start: %zu, y_start: %zu, x_limit: %zu, y_limit: %zu\n",
               x_start, y_start, x_limit, y_limit);

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
                    printf("HERE\n");
                    List *list = s->el;
                    Item *items_list = list->items;

                    char *item;
                    while (items_list != NULL)
                    {
                        item = items_list->text;
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
                else if (s->type == INPUT)
                {
                    Pixel *temp = &((*pixels)[y][x]);

                    if (y == y_start || y == y_limit - 1)
                    {
                        temp->pixel = '*';
                    }
                    else if (x == x_start || x == x_limit - 1)
                    {
                        temp->pixel = '*';
                    }
                    else
                    {
                        temp->pixel = ' ';
                    }

                    temp->active = ACTIVE;
                    temp->focused = UNFOCUSED;
                    temp->type = SECTION;
                }
                else
                {
                    EXIT_ON_ERROR("Section type undefined");
                    break;
                }
            }
        }

        s = s->next;
        completed = false;
    }
}

void update_widget(Pixel ***pixels, Widget *curr_widget, size_t window_width, size_t window_height)
{
    // Update pixels for list of sections in current widget
    update_sections(pixels, curr_widget);

    size_t y_start = curr_widget->pos_y;
    size_t x_start = curr_widget->pos_x;
    size_t y_limit = curr_widget->height + curr_widget->pos_y;
    size_t x_limit = curr_widget->width + curr_widget->pos_x;

    if (window_width % 2 != 0)
    {
        x_limit++;
        if (x_limit >= window_width + 1)
        {
            x_limit--;
        }
    }

    if (window_height % 2 != 0)
    {
        y_limit++;
        if (y_limit >= window_height + 1)
        {
            y_limit--;
        }
    }

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
}

void update_window(Window window)
{
    Widget *w = window.widgets;
    while (w != NULL)
    {
        // printf("width: %zu, height: %zu, pos_x: %zu, pos_y: %zu\n",
        //        widgets->width, widgets->height, widgets->pos_x, widgets->pos_y);
        if (w->width < window.width && w->height < window.height)
        {
            update_widget(&window.pixels, w, window.width, window.height);
        }
        else
        {
            EXIT_ON_ERROR_PAR("Widget size is bigger than window size. Max width: %zu, Max height %zu, Widget widht: %zu, height: %zu\n", window.width, window.height, w->width, w->height);
        }

        w = w->next;
    }
}

// void clean_element(Pixel ***pixels, Widget *curr_widget, Section *curr_section, int focus_index)
// {
//     size_t y_clean_start = 0;
//     size_t y_clean_end = 0;
//     size_t x_clean_start = 0;
//     size_t x_clean_end = 0;

//     if (curr_section->type == LIST)
//     {
//         List *list = curr_section->el;
//         y_clean_start = curr_widget->pos_y + curr_section->pos_y + focus_index + 1;
//         y_clean_end = y_clean_start + 1;
//         x_clean_start = curr_widget->pos_x + curr_section->pos_x + 1;
//         x_clean_end = x_clean_start + strlen(get(list->items, focus_index));

//         // Clean section pixels
//         for (size_t y = y_clean_start; y < y_clean_end; y++)
//         {

//             for (size_t x = x_clean_start; x < x_clean_end; ++x)
//             {
//                 ((*pixels)[y][x]).focused = UNFOCUSED;
//             }
//         }
//     }
//     else if (curr_section->type == INPUT)
//     {
//         y_clean_start = (curr_widget->pos_y + curr_section->pos_y) + 1;
//         y_clean_end = y_clean_start + curr_section->height;
//         x_clean_start = curr_widget->pos_x + curr_section->pos_x + 1;
//         x_clean_end = x_clean_start + curr_section->width;

//         // Set new focused pixels
//         for (size_t y = y_clean_start; y < y_clean_end; y++)
//         {
//             for (size_t x = x_clean_start; x < x_clean_end; ++x)
//             {
//                 Pixel *temp = &((*pixels)[y][x]);

//                 if ((y == y_clean_start || y == y_clean_end - 1) || (x == x_clean_start || x == x_clean_end - 1))
//                 {
//                     temp->focused = UNFOCUSED;
//                 }
//             }
//         }
//     }
//     else
//     {
//         EXIT_ON_ERROR("Section type undefined for new_section");
//     }

//     // printf("CLEAR\nCurrent section: %s\n", curr_section->label);

//     // printf("x_clean_start: %zu, x_clean_end: %zu, y_clean_start: %zu, x_clean_end: %zu\n",
//     //        x_clean_start, x_clean_end, y_clean_start, y_clean_end);
// }

// void clean_section(Pixel ***pixels, Widget *curr_widget, Section *curr_section)
// {

//     size_t y_clean_start = curr_widget->pos_y + curr_section->pos_y + 1;
//     size_t y_clean_end = y_clean_start + curr_section->height;
//     size_t x_clean_start = curr_widget->pos_x + curr_section->pos_x + 1;
//     size_t x_clean_end = x_clean_start + curr_section->width;

//     // printf("x_clean_start: %zu, x_clean_end: %zu, y_clean_start: %zu, x_clean_end: %zu\n",
//     //        x_clean_start, x_clean_end, y_clean_start, y_clean_end);

//     for (size_t y = y_clean_start; y < y_clean_end; y++)
//     {

//         for (size_t x = x_clean_start; x < x_clean_end; ++x)
//         {
//             ((*pixels)[y][x]).focused = UNFOCUSED;
//             ((*pixels)[y][x]).active = INACTIVE;
//             ((*pixels)[y][x]).pixel = ' ';
//         }
//     }
// }

// bool change_focused_element(Pixel ***pixels, Widget *curr_widget, Section *curr_section,
//                             int *focus_index_ptr, int dir)
// {
//     // printf("Current section: %s\n", curr_section->label);

//     // printf("focus_index: %d, pos_x: %zu, pos_y: %zu\n",
//     //        *focus_index_ptr, curr_section->pos_x, curr_section->pos_y);

//     // Get last section before changing widget

//     int focus_index = *focus_index_ptr;

//     size_t y_new_start;
//     size_t y_new_end;
//     size_t x_new_start;
//     size_t x_new_end;

//     if (curr_section->type == LIST)
//     {
//         List *list = curr_section->el;

//         if (focus_index > (int)list->item_count)
//         {
//             *focus_index_ptr = list->item_count - 1;
//             focus_index = list->item_count - 1;
//         }
//         else if (focus_index == (int)list->item_count || focus_index < 0)
//         {
//             return false;
//         }

//         y_new_start = (curr_widget->pos_y + curr_section->pos_y) + focus_index + 1;
//         y_new_end = y_new_start + 1;
//         x_new_start = curr_widget->pos_x + curr_section->pos_x + 1;
//         x_new_end = x_new_start + strlen(get(list->items, focus_index));

//         if (dir == NEXT && focus_index - 1 >= 0)
//         {
//             // Not first or last element, can clean previous element
//             clean_element(pixels, curr_widget, curr_section, focus_index - 1);
//         }
//         else if (dir == PREVIOUS && focus_index + 1 <= (int)list->item_count)
//         {
//             clean_element(pixels, curr_widget, curr_section, focus_index + 1);
//         }
//         else if (dir == CURRENT)
//         {
//             clean_element(pixels, curr_widget, curr_section, focus_index);
//         }

//         // Set new focused pixels
//         for (size_t y = y_new_start; y < y_new_end; y++)
//         {
//             for (size_t x = x_new_start; x < x_new_end; ++x)
//             {
//                 ((*pixels)[y][x]).focused = FOCUSED;
//             }
//         }
//     }
//     else if (curr_section->type == INPUT)
//     {
//         y_new_start = (curr_widget->pos_y + curr_section->pos_y) + 1;
//         y_new_end = y_new_start + curr_section->height;
//         x_new_start = curr_widget->pos_x + curr_section->pos_x + 1;
//         x_new_end = x_new_start + curr_section->width;

//         // Set new focused pixels
//         for (size_t y = y_new_start; y < y_new_end; y++)
//         {
//             for (size_t x = x_new_start; x < x_new_end; ++x)
//             {
//                 Pixel *temp = &((*pixels)[y][x]);

//                 if ((y == y_new_start || y == y_new_end - 1) || (x == x_new_start || x == x_new_end - 1))
//                 {
//                     temp->focused = FOCUSED;
//                 }
//             }
//         }
//     }
//     else
//     {
//         EXIT_ON_ERROR("Section type undefined");
//     }

//     // printf("x_new_start: %zu, x_new_end: %zu, y_new_start: %d, x_new_end: %zu\n",
//     //        x_new_start, x_new_end, y_new_start, y_new_end);

//     return true;
// }

// Section *get_next_editable_section(Widget *curr_widget, Section *curr_section)
// {
//     // printf("NEXT\n-------------------------------------------------------\n");
//     // printf("For widget id: %d\n", curr_widget->id);
//     Section *s = curr_section;

//     Section *temp;
//     while (s != NULL)
//     {
//         temp = s;
//         // printf("Section label: %s, Section id: %d, Current Section: %d\n", s->label, s->id, curr_widget->current_section);
//         if (temp->state == EDITABLE && (s->id != curr_widget->current_section))
//         {
//             curr_widget->current_section = s->id;
//             // printf("-------------------------------------------------------\nEND_NEXT\n");
//             return temp;
//         }

//         s = s->next;
//     }

//     // printf("-------------------------------------------------------\nEND_NEXT\n");
//     return NULL;
// }

// Section *get_prev_editable_section(Widget *curr_widget, Section *curr_section)
// {
//     // printf("PREV\n-------------------------------------------------------\n");
//     // printf("For widget id: %d\n", curr_widget->id);

//     Section *s = curr_section;
//     Section *temp;

//     while (s != NULL)
//     {
//         temp = s;

//         // printf("Section label: %s, Section id: %d, Current Section: %d\n", s->label, s->id, curr_widget->current_section);
//         if (temp->state == EDITABLE && (s->id != curr_widget->current_section))
//         {
//             curr_widget->current_section = s->id;
//             // printf("-------------------------------------------------------\nEND_PREV\n");

//             return temp;
//         }

//         s = s->prev;
//     }

//     // printf("-------------------------------------------------------\nEND_PREV\n");
//     return NULL;
// }

// bool action(Pixel ***pixels, Widget *curr_widget, Section **curr_section_ptr,
//             int *focus_index, size_t window_width, size_t window_height)
// {

//     Section *curr_section = *curr_section_ptr;
//     if (curr_section->state == STATIC)
//     {
//         return false;
//     }

//     if (curr_section->type == LIST)
//     {
//         List *list = curr_section->el;

//         if (list->target == NULL)
//         {
//             EXIT_ON_ERROR("target not valid for list");
//         }

//         Section *target = list->target;
//         List *target_list = target->el;

//         // printf("Current: %s, Target: %s\n", curr_section->label, target->label);

//         char *item = get(list->items, *focus_index);
//         size_t item_len = strlen(item);

//         if (target_list->type == CHECK_BOX && list->type == CHECK_BOX)
//         {
//             if (list->checked)
//             {
//                 item[3] = ' ';
//                 list->checked = UNCHECKED;
//             }
//             else
//             {
//                 item[3] = 'x';
//                 list->checked = CHECKED;
//             }
//         }

//         delete (&list->items, *focus_index);
//         add(target_list->items, item);

//         // Clean section pixels
//         clean_section(pixels, curr_widget, curr_section);

//         curr_section->height--;
//         target->height++;

//         target->width = item_len > target->width ? item_len : target->width;

//         // Update widget
//         update_widget(pixels, curr_widget, window_width, window_height);

//         // Update list count
//         target_list->item_count++;
//         list->item_count--;
//         int temp = target_list->item_count - 1;
//         (*focus_index) = temp;
//         change_focused_element(pixels, curr_widget, target, &temp, NONE);
//         *curr_section_ptr = target;
//     }
//     else
//     {
//         EXIT_ON_ERROR("Section type undefined");
//     }

//     return true;
// }

// bool type(Pixel ***pixels, Widget *curr_widget, Section *curr_section, char key)
// {

//     Input *input = curr_section->el;
//     size_t global_cursor_x = curr_widget->pos_x + curr_section->pos_x + input->cursor_x + 2;
//     size_t global_cursor_y = curr_widget->pos_y + curr_section->pos_y + input->cursor_y + 2;

//     switch (key)
//     {
//     case 8:
//         // Backspace
//         if (input->cursor_x == 0)
//         {
//             return false;
//         }

//         ((*pixels)[global_cursor_y][global_cursor_x - 1]).pixel = ' ';
//         input->cursor_x--;
//         return true;
//     case 13:
//         // enter
//         if ((size_t)input->cursor_y + 1 < curr_section->height - 2)
//         {
//             input->cursor_y++;
//             return true;
//         }
//         else
//         {
//             return false;
//         }
//     default:
//         break;
//     }

//     // printf("key: \"%c\"\n", key);

//     ((*pixels)[global_cursor_y][global_cursor_x]).pixel = key;

//     // printf("cursor_x: %d, cursor_y: %d\n", input->cursor_x, input->cursor_y);
//     // printf("global_cursor_x: %d, global_cursor_y: %d\n", global_cursor_x, global_cursor_y);

//     // printf("change line at: %zu\n", curr_widget->pos_x + curr_section->pos_x + curr_section->width - 1);
//     if (global_cursor_x == curr_widget->pos_x + curr_section->pos_x + curr_section->width - 1)
//     {
//         input->cursor_x = 0;
//         if ((size_t)input->cursor_y + 1 < curr_section->height - 2)
//         {
//             input->cursor_y++;
//             return true;
//         }
//     }
//     else
//     {
//         input->cursor_x++;
//         return true;
//     }

//     return false;
// }

void render_debug(Pixel **pixels, size_t window_width, size_t window_height)
{
    printf("DEBUG_STATE: %d\n", DEBUG_STATE);
    DEBUG_STATE++;
    for (size_t y = 0; y < (size_t)window_height; y++)
    {
        for (size_t x = 0; x < (size_t)window_width; x++)
        {
            Pixel curr_pixel = pixels[y][x];

            if (curr_pixel.focused)
            {
                printf("%c", '+');
            }
            // else
            // if (!curr_pixel.active)
            // {
            //     printf("%c", 'x');
            // }
            else
            {
                printf("%c", curr_pixel.pixel);
            }
        }
    }
}

void render(Window w)
{
    // system("cls");

    for (size_t y = 0; y < w.height; y++)
    {
        for (size_t x = 0; x < w.width; x++)
        {
            Pixel curr_pixel = w.pixels[y][x];

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
    char *todo_array[] = {"Buy a bread", "Eat", "Drink code"};
    size_t todo_array_len = sizeof(todo_array) / sizeof(todo_array[0]);
    
    char *done_array[] = {"Sleep", "Code"};
    size_t done_array_len = sizeof(done_array) / sizeof(done_array[0]);
    (void)done_array_len;
    // Create main window
    Window window = create_window("Main", 0);
    
    Item *todo_items = create_items(todo_array, todo_array_len);
    Label todo_label = create_label("TODO");
    Section todo_label_section = create_section(&todo_label, LABEL, 20, 1, 0, 0, 0, "TODO_LABEL");
    List todo_list = create_list(todo_items, CHECK_BOX, false, NULL);
    Section todo_list_section = create_section(&todo_list, LIST, 10, 3, 0, 1, 0, "TODO_LIST");
    link_sections(&todo_label_section, &todo_list_section);
    
    Widget todo_widget = create_widget('*', 1, 60, 15, 0, 0, 0, &todo_label_section);

    // Item *done_items = create_items(done_array, done_array_len);
    // Label done_label = create_label("DONE");
    // Section done_label_section = create_section(&done_label, LABEL, 20, 1, 0, 0, 0, "done_LABEL");
    // List done_list = create_list(done_items, CHECK_BOX, false, NULL);
    // Section done_list_section = create_section(&done_list, LIST, 20, 1, 0, 1, 0, "done_LIST");
    // link_sections(&done_label_section, &done_list_section);

    // Widget done_widget = create_widget('*', 1, 58, 15, 60, 1, 0, &done_label_section);
    // link_widgets(&todo_widget, &done_widget);

    window.widgets = &todo_widget;

    update_window(window);
    render(window);

    // Section done_label_section;
    // Label done_label;
    // Section done_list_section;
    // List done_list;

    // Widget input_widget;
    // Section input_section;
    // Input input;

    return 0;
}