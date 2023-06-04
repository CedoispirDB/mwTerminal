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
#define TEXT_BOX 2
#define INPUT 3

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
typedef struct Section Section;
typedef struct Widget Widget;

struct Widget
{
    size_t width;
    size_t height;
    size_t pos_x;
    size_t pos_y;
    size_t id;
    Section *sections;
    Widget *next;
    Widget *prev;
    int current_section;
    Section *last_section;
};

struct Section
{
    void *el;
    section_state state;
    section_type type;
    size_t width;
    size_t height;
    size_t pos_x;
    size_t pos_y;
    int id;
    Section *next;
    Section *prev;
    char *label;
};

typedef struct List
{
    LinkedList *items;
    size_t min_width;
    size_t item_count;
    Section *target;
    bool checked;
    list_type type;
} List;

typedef struct TextBox
{
    char *content;
} TextBox;

typedef struct Label
{
    char *content;
} Label;

typedef struct Input
{
    char *buf;
} Input;

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
                        size_t id, Section *sections)
{
    Widget *w = malloc(sizeof(Widget));

    w->width = width;
    w->height = height;
    w->pos_x = pos_x;
    w->pos_y = pos_y;
    w->sections = sections;
    w->id = id;
    w->next = NULL;
    w->prev = NULL;
    w->current_section = -1;
    w->last_section = NULL;

    return w;
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

Section *allocate_section(void *el, section_type type, size_t width, size_t heigth, size_t pos_x, size_t pos_y, int id, char *label)
{
    Section *s = malloc(sizeof(Section));
    s->el = el;
    s->type = type;
    s->pos_x = pos_x;
    s->pos_y = pos_y;
    s->width = width;
    s->height = heigth;
    s->id = id;
    s->label = label;
    s->next = NULL;
    s->prev = NULL;

    switch (type)
    {
    case LIST:
        s->state = EDITABLE;
        break;
    case LABEL:
        s->state = STATIC;
        break;
    case TEXT_BOX:
        s->state = STATIC;
        break;
    case INPUT:
        s->state = EDITABLE;
        break;
    default:
        EXIT_ON_ERROR("Section type undefined");
        break;
    }

    return s;
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

List *allocate_list(LinkedList *items, size_t min_width, list_type type, bool checked, Section *section)
{
    List *list = malloc(sizeof(List));
    list->items = items;
    list->min_width = min_width;
    list->item_count = (size_t)len(items);
    list->target = section;
    list->type = type;
    list->checked = checked;

    if (type == CHECK_BOX)
    {
        LinkedList *temp = items;
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
            char *value = temp->value;

            size_t prefixLength = strlen(prefix);
            size_t targetLength = strlen(value);

            size_t newLength = prefixLength + targetLength + 1;

            char *newString = malloc(newLength);

            strcpy(newString, prefix);

            strcat(newString, value);
            temp->value = newString;
            temp = temp->next;
        }
    }

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
        // printf("s width: %zu, s height: %zu, s pos_x: %zu, s pos_y: %zu\n",
        //        s->width, s->height, s->pos_x, s->pos_y);
        // printf("w width: %zu, w height: %zu, w pos_x: %zu, w pos_y: %zu\n",
        //        w->width, w->height, w->pos_x, w->pos_y);
        // printf("Section type: %d\n", s->type);
        size_t y_start = w->pos_y + s->pos_y + 1;
        size_t x_start = w->pos_x + s->pos_x + 1;
        size_t y_limit = y_start + s->height;
        size_t x_limit = x_start + s->width;

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

void update_pixels(Pixel ***pixels, Widget *widgets, size_t window_width, size_t window_height)
{

    (void)window_height;

    while (widgets != NULL)
    {
        // printf("width: %zu, height: %zu, pos_x: %zu, pos_y: %zu\n",
        //        widgets->width, widgets->height, widgets->pos_x, widgets->pos_y);
        update_widget(pixels, widgets, window_width, window_height);

        widgets = widgets->next;
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
        y_clean_start = curr_widget->pos_y + curr_section->pos_y + focus_index + 1;
        y_clean_end = y_clean_start + 1;
        x_clean_start = curr_widget->pos_x + curr_section->pos_x + 1;
        x_clean_end = x_clean_start + strlen(get(list->items, focus_index));

        // Clean section pixels
        for (size_t y = y_clean_start; y < y_clean_end; y++)
        {

            for (size_t x = x_clean_start; x < x_clean_end; ++x)
            {
                ((*pixels)[y][x]).focused = UNFOCUSED;
            }
        }
    }
    else if (curr_section->type == INPUT)
    {
        y_clean_start = (curr_widget->pos_y + curr_section->pos_y) + 1;
        y_clean_end = y_clean_start + curr_section->height;
        x_clean_start = curr_widget->pos_x + curr_section->pos_x + 1;
        x_clean_end = x_clean_start + curr_section->width;

        // Set new focused pixels
        for (size_t y = y_clean_start; y < y_clean_end; y++)
        {
            for (size_t x = x_clean_start; x < x_clean_end; ++x)
            {
                Pixel *temp = &((*pixels)[y][x]);

                if ((y == y_clean_start || y == y_clean_end - 1) || (x == x_clean_start || x == x_clean_end - 1))
                {
                    temp->focused = UNFOCUSED;
                }
            }
        }
    }
    else
    {
        EXIT_ON_ERROR("Section type undefined for new_section");
    }

    // printf("CLEAR\nCurrent section: %s\n", curr_section->label);

    // printf("x_clean_start: %zu, x_clean_end: %zu, y_clean_start: %zu, x_clean_end: %zu\n",
    //        x_clean_start, x_clean_end, y_clean_start, y_clean_end);
}

void clean_section(Pixel ***pixels, Widget *curr_widget, Section *curr_section)
{

    size_t y_clean_start = curr_widget->pos_y + curr_section->pos_y + 1;
    size_t y_clean_end = y_clean_start + curr_section->height;
    size_t x_clean_start = curr_widget->pos_x + curr_section->pos_x + 1;
    size_t x_clean_end = x_clean_start + curr_section->width;

    // printf("x_clean_start: %zu, x_clean_end: %zu, y_clean_start: %zu, x_clean_end: %zu\n",
    //        x_clean_start, x_clean_end, y_clean_start, y_clean_end);

    for (size_t y = y_clean_start; y < y_clean_end; y++)
    {

        for (size_t x = x_clean_start; x < x_clean_end; ++x)
        {
            ((*pixels)[y][x]).focused = UNFOCUSED;
            ((*pixels)[y][x]).active = INACTIVE;
            ((*pixels)[y][x]).pixel = ' ';
        }
    }
}

bool change_focused_element(Pixel ***pixels, Widget *curr_widget, Section *curr_section,
                            int *focus_index_ptr, int dir)
{
    // printf("Current section: %s\n", curr_section->label);

    // printf("focus_index: %d, pos_x: %zu, pos_y: %zu\n",
    //        *focus_index_ptr, curr_section->pos_x, curr_section->pos_y);

    // Get last section before changing widget

    int focus_index = *focus_index_ptr;

    size_t y_new_start;
    size_t y_new_end;
    size_t x_new_start;
    size_t x_new_end;

    if (curr_section->type == LIST)
    {
        List *list = curr_section->el;

        if (focus_index > (int)list->item_count)
        {
            *focus_index_ptr = list->item_count - 1;
            focus_index = list->item_count - 1;
        }
        else if (focus_index == (int)list->item_count || focus_index < 0)
        {
            return false;
        }

        y_new_start = (curr_widget->pos_y + curr_section->pos_y) + focus_index + 1;
        y_new_end = y_new_start + 1;
        x_new_start = curr_widget->pos_x + curr_section->pos_x + 1;
        x_new_end = x_new_start + strlen(get(list->items, focus_index));

        if (dir == NEXT && focus_index - 1 >= 0)
        {
            // Not first or last element, can clean previous element
            clean_element(pixels, curr_widget, curr_section, focus_index - 1);
        }
        else if (dir == PREVIOUS && focus_index + 1 <= (int)list->item_count)
        {
            clean_element(pixels, curr_widget, curr_section, focus_index + 1);
        }
        else if (dir == CURRENT)
        {
            clean_element(pixels, curr_widget, curr_section, focus_index);
        }

        // Set new focused pixels
        for (size_t y = y_new_start; y < y_new_end; y++)
        {
            for (size_t x = x_new_start; x < x_new_end; ++x)
            {
                ((*pixels)[y][x]).focused = FOCUSED;
            }
        }
    }
    else if (curr_section->type == INPUT)
    {
        y_new_start = (curr_widget->pos_y + curr_section->pos_y) + 1;
        y_new_end = y_new_start + curr_section->height;
        x_new_start = curr_widget->pos_x + curr_section->pos_x + 1;
        x_new_end = x_new_start + curr_section->width;

        // Set new focused pixels
        for (size_t y = y_new_start; y < y_new_end; y++)
        {
            for (size_t x = x_new_start; x < x_new_end; ++x)
            {
                Pixel *temp = &((*pixels)[y][x]);

                if ((y == y_new_start || y == y_new_end - 1) || (x == x_new_start || x == x_new_end - 1))
                {
                    temp->focused = FOCUSED;
                }
            }
        }
    }
    else
    {
        EXIT_ON_ERROR("Section type undefined");
    }

    // printf("x_new_start: %zu, x_new_end: %zu, y_new_start: %d, x_new_end: %zu\n",
    //        x_new_start, x_new_end, y_new_start, y_new_end);

    return true;
}

Section *get_next_editable_section(Widget *curr_widget, Section *curr_section)
{
    // printf("NEXT\n-------------------------------------------------------\n");
    // printf("For widget id: %d\n", curr_widget->id);
    Section *s = curr_section;

    Section *temp;
    while (s != NULL)
    {
        temp = s;
        // printf("Section label: %s, Section id: %d, Current Section: %d\n", s->label, s->id, curr_widget->current_section);
        if (temp->state == EDITABLE && (s->id != curr_widget->current_section))
        {
            curr_widget->current_section = s->id;
            // printf("-------------------------------------------------------\nEND_NEXT\n");
            return temp;
        }

        s = s->next;
    }

    // printf("-------------------------------------------------------\nEND_NEXT\n");
    return NULL;
}

Section *get_prev_editable_section(Widget *curr_widget, Section *curr_section)
{
    // printf("PREV\n-------------------------------------------------------\n");
    // printf("For widget id: %d\n", curr_widget->id);

    Section *s = curr_section;
    Section *temp;

    while (s != NULL)
    {
        temp = s;

        // printf("Section label: %s, Section id: %d, Current Section: %d\n", s->label, s->id, curr_widget->current_section);
        if (temp->state == EDITABLE && (s->id != curr_widget->current_section))
        {
            curr_widget->current_section = s->id;
            // printf("-------------------------------------------------------\nEND_PREV\n");

            return temp;
        }

        s = s->prev;
    }

    // printf("-------------------------------------------------------\nEND_PREV\n");
    return NULL;
}

bool action(Pixel ***pixels, Widget *curr_widget, Section **curr_section_ptr,
            int *focus_index, size_t window_width, size_t window_height)
{

    Section *curr_section = *curr_section_ptr;
    if (curr_section->state == STATIC)
    {
        return false;
    }

    if (curr_section->type == LIST)
    {
        List *list = curr_section->el;

        if (list->target == NULL)
        {
            EXIT_ON_ERROR("target not valid for list");
        }

        Section *target = list->target;
        List *target_list = target->el;

        // printf("Current: %s, Target: %s\n", curr_section->label, target->label);

        char *item = get(list->items, *focus_index);
        size_t item_len = strlen(item);

        if (target_list->type == CHECK_BOX && list->type == CHECK_BOX)
        {
            if (list->checked)
            {
                item[3] = ' ';
                list->checked = UNCHECKED;
            }
            else
            {
                item[3] = 'x';
                list->checked = CHECKED;
            }
        }

        delete (&list->items, *focus_index);
        add(target_list->items, item);

        // Clean section pixels
        clean_section(pixels, curr_widget, curr_section);

        curr_section->height--;
        target->height++;

        target->width = item_len > target->width ? item_len : target->width;

        // Update widget
        update_widget(pixels, curr_widget, window_width, window_height);

        // Update list count
        target_list->item_count++;
        list->item_count--;
        int temp = target_list->item_count - 1;
        (*focus_index) = temp;
        change_focused_element(pixels, curr_widget, target, &temp, NONE);
        *curr_section_ptr = target;
    }
    else
    {
        EXIT_ON_ERROR("Section type undefined");
    }

    return true;
}

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

void render(Pixel **pixels, size_t window_width, size_t window_height)
{
    system("cls");

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
    todo_items[0] = createStr("Buy a bread");
    todo_items[1] = createStr("Eat");
    todo_items[2] = createStr("Drink code");

    LinkedList *todo = convertArray(todo_items, todo_items_len);
    size_t todo_min_width = calculate_min_width(todo) + 6;

    char **done_items;
    size_t done_items_len = 3;
    done_items = (char **)malloc(done_items_len * sizeof(char *));
    done_items[0] = createStr("sleep");
    done_items[1] = createStr("high");
    done_items[2] = createStr("low");

    LinkedList *done = convertArray(done_items, done_items_len);
    size_t done_min_width = calculate_min_width(done) + 6;

    // Create window
    int window_width, window_height;
    Pixel **pixels;
    create_window(&pixels, &window_width, &window_height);
    printf("Window size: %d window_width x %d window_height\n", window_width, window_height);

    size_t widget_width = (size_t)window_width / 2;
    size_t widget_height = (size_t)window_height;

    int gap = 2;
    int todo_pos_x = (widget_width - todo_min_width - done_min_width - gap) / 2;
    int done_pos_x = todo_pos_x + todo_min_width + gap;

    int todo_label_name_len = 4;
    int todo_label_d = (todo_min_width - todo_label_name_len) / 2;
    int todo_label_pos_x = todo_label_d + todo_pos_x;

    int done_label_name_len = 5;
    int done_label_d = (done_min_width - done_label_name_len) / 2;
    int done_label_pos_x = todo_label_pos_x + todo_label_name_len + todo_label_d + gap + done_label_d;

    // printf("todo label pos: %d, done label pos: %d\n", todo_label_pos_x, done_label_pos_x);
    // printf("min width done: %zu\n", done_min_width);

    // -------------------------
    // -   Create todo widget  -
    // -------------------------

    // Create todo list section
    List *todo_list = allocate_list(todo, todo_min_width, CHECK_BOX, UNCHECKED, NULL);
    Section *todo_section = allocate_section(todo_list, LIST, todo_min_width, todo_items_len, todo_pos_x, 1, 0, "TODO_LIST");

    // Create todo label section
    Label *todo_label = allocate_label("TODO");
    Section *todo_label_section = allocate_section(todo_label, LABEL, todo_label_name_len, 1, todo_label_pos_x, 0, 1, "TODO_LABEL");

    // Create done widget
    // Create done list section
    List *done_list = allocate_list(done, done_min_width, CHECK_BOX, CHECKED, todo_section);
    Section *done_section = allocate_section(done_list, LIST, done_min_width, done_items_len, done_pos_x, 1, 2, "DONE_LIST");
    todo_list->target = done_section;

    // Create done label section
    Label *done_label = allocate_label(" DONE");
    Section *done_label_section = allocate_section(done_label, LABEL, done_label_name_len, 1, done_label_pos_x, 0, 3, "DONE_LABEL");

    // Create Input section
    Input *input = malloc(sizeof(Input));
    Section *input_section = allocate_section(input, INPUT, 30, 3, 10, 3, 0, "INPUT_BOX");
    // Link sections together
    link_sections(todo_label_section, todo_section);
    link_sections(todo_label_section, done_label_section);
    link_sections(todo_label_section, done_section);

    Widget *w = allocate_widget(widget_width, widget_height,
                                0, 0,
                                0, todo_label_section);
    Widget *w2 = allocate_widget(widget_width, widget_height,
                                 widget_width, 0,
                                 1, input_section);
    // Widget *w3 = allocate_widget(widget_width, widget_height,
    //                              0, widget_height,
    //                              2, NULL);
    // Widget *w4 = allocate_widget(widget_width, widget_height,
    //                              widget_width, widget_height,
    //                              3, NULL);

    link_widgets(w, w2);
    // link_widgets(w, w3);
    // link_widgets(w, w4);

    update_pixels(&pixels, w, window_width, window_height);

    hideCursor();

    int focus_index = 0;

    Widget *curr_widget = w;
    Section *curr_widget_sections = curr_widget->sections;

    Section *curr_section = get_next_editable_section(curr_widget, curr_widget_sections);

    // printf("Current section: %s\n", curr_section->label);
    // Select initial element
    change_focused_element(&pixels, curr_widget, curr_section, &focus_index, NONE);

    char key;
    int arrowCode;
    bool need_to_render = true;

    while (key != 'q')
    {
        if (need_to_render)
        {
            // printf("------------------------------------\n");
            // printf("Widget id: %zu\n", curr_widget->id);
            // printf("Current section label: %s, id: %d\n", curr_section->label, curr_section->id);
            // printf("Widget current section: %d\n", curr_widget->current_section);
            // printf("------------------------------------\n");
            render(pixels, window_width, window_height);
            need_to_render = false;
        }

        key = _getch();

        switch (key)
        {
        case -32:

            if (curr_section != NULL)
            {
                arrowCode = _getch();

                if (arrowCode == 80)
                {
                    // Arrow down
                    int temp_index = focus_index + 1;
                    need_to_render = change_focused_element(&pixels, curr_widget, curr_section, &temp_index, NEXT);
                    if (need_to_render)
                    {
                        focus_index++;
                    }
                }
                else if (arrowCode == 72)
                {
                    // Arrow up
                    int temp_index = focus_index - 1;
                    need_to_render = change_focused_element(&pixels, curr_widget, curr_section, &temp_index, PREVIOUS);
                    if (need_to_render)
                    {
                        focus_index--;
                    }
                }
            }

            break;
        case 9:
            // Tab
            if (GetKeyState(VK_SHIFT) & 0x8000)
            {
                //  Shift + Tab
                Section *temp = curr_section;
                int temp_index = focus_index;
                curr_section = get_prev_editable_section(curr_widget, temp);

                if (curr_section != NULL)
                {
                    need_to_render = change_focused_element(&pixels, curr_widget, curr_section, &focus_index, NONE);
                    clean_element(&pixels, curr_widget, temp, temp_index);
                    need_to_render = true;
                }
                else
                {
                    curr_section = temp;

                    if (curr_widget->prev != NULL)
                    {
                        // Clean previous section before moving to next widget
                        clean_element(&pixels, curr_widget, curr_section, focus_index);

                        curr_widget->current_section--;

                        curr_widget = curr_widget->prev;
                        curr_section = get_prev_editable_section(curr_widget, curr_widget->last_section);

                        if (curr_section == NULL)
                        {
                            EXIT_ON_ERROR("Current section is NULL when trying to go to previous widget");
                        }

                        // printf("prev current widget id: %d\n", curr_widget->id);
                        // printf("prev current widget current section: %d\n", curr_widget->current_section);
                        // printf("prev section for next widget: %s\n", curr_section->label);

                        need_to_render = change_focused_element(&pixels, curr_widget, curr_section, &focus_index, NONE);
                    }
                }
                break;
            }

            Section *temp = curr_section;
            int temp_index = focus_index;
            curr_section = get_next_editable_section(curr_widget, temp);
            curr_widget->last_section = curr_section;

            if (curr_section != NULL)
            {
                need_to_render = change_focused_element(&pixels, curr_widget, curr_section, &focus_index, NONE);
                clean_element(&pixels, curr_widget, temp, temp_index);
                need_to_render = true;
            }
            else
            {
                curr_section = temp;
                curr_widget->last_section = curr_section;

                if (curr_widget->next != NULL)
                {
                    // Clean previous section before moving to next widget
                    clean_element(&pixels, curr_widget, curr_section, focus_index);

                    curr_widget->current_section--;

                    curr_widget = curr_widget->next;
                    curr_section = get_next_editable_section(curr_widget, curr_widget->sections);

                    if (curr_section == NULL)
                    {
                        EXIT_ON_ERROR("Current section is NULL when trying to go to next widget");
                    }

                    // printf("New current widget id: %d\n", curr_widget->id);
                    // printf("New current widget current section: %d\n", curr_widget->current_section);
                    // printf("New section for next widget: %s\n", curr_section->label);
                    need_to_render = change_focused_element(&pixels, curr_widget, curr_section, &focus_index, NONE);
                }
            }

            break;
        case '\r':
            need_to_render = action(&pixels, curr_widget, &curr_section, &focus_index, window_width, window_height);
            printf("Focus index: %d\n", focus_index);
            break;
        default:
            break;
        }
    }

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