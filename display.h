extern int harangue_window_rows;
extern int harangue_window_cols;

#define ATTR_RESET      0
#define ATTR_NONE       0
#define ATTR_BRIGHT     1
#define ATTR_BOLD       1
#define ATTR_DIM        2
#define ATTR_UNDERLINE  4
#define ATTR_BLINK      5
#define ATTR_REVERSE    7
#define ATTR_HIDDEN     8

#define ATTR_NORMAL    22 // Not Bold or Dim
#define ATTR_NOLINE    24 // Not Underlined
#define ATTR_STEADY    25 // Not Blinking
#define ATTR_POSITIVE  27 // Not Reversed / Inversed
#define ATTR_VISIBLE   28 // Not Hidden


#define ATTR_FG_BLACK   30
#define ATTR_FG_RED     31
#define ATTR_FG_GREEN   32
#define ATTR_FG_YELLOW  33
#define ATTR_FG_BLUE    34
#define ATTR_FG_MAGENTA 35
#define ATTR_FG_CYAN    36
#define ATTR_FG_WHITE   37
#define ATTR_FG_DEFAULT 39

#define ATTR_BG_BLACK   40
#define ATTR_BG_RED     41
#define ATTR_BG_GREEN   42
#define ATTR_BG_YELLOW  43
#define ATTR_BG_BLUE    44
#define ATTR_BG_MAGENTA 45
#define ATTR_BG_CYAN    46
#define ATTR_BG_WHITE   47
#define ATTR_BG_DEFAULT 49

#define COLOR_BLACK    0
#define COLOR_RED      1
#define COLOR_GREEN    2
#define COLOR_YELLOW   3
#define COLOR_BLUE     4
#define COLOR_MAGENTA  5
#define COLOR_CYAN     6
#define COLOR_WHITE    7

void harangue_display_setup(void);
void harangue_display_cleanup(void);
void harangue_display_reset(void);

void harangue_display_cursor_move(int row, int col);
void harangue_display_cursor_up(int count);
void harangue_display_cursor_down(int count);
void harangue_display_cursor_forward(int count);
void harangue_display_cursor_backward(int count);
void harangue_display_cursor_save(void);
void harangue_display_cursor_restore(void);

void harangue_display_scroll_set(int start, int end);
void harangue_display_scroll_down(void);
void harangue_display_scroll_up(void);

void harangue_display_erase_to_end(void);
void harangue_display_erase_to_start(void);
void harangue_display_erase_line(void);
void harangue_display_erase_down(void);
void harangue_display_erase_up(void);
void harangue_display_erase_screen(void);

void harangue_display_attribute(int attr);
void harangue_display_attributes(int attr, int fore, int back);

void harangue_display_256color_foreground(int color);
void harangue_display_256color_background(int color);
void harangue_display_256color_attributes(int attr, int fore, int back);

void harangue_display_special(char *s);
void harangue_display_bell(void);

void harangue_display_line_doubled_top(void);
void harangue_display_line_doubled_bottom(void);
void harangue_display_line_double_width(void);

void harangue_display_set_title(char* s);
void harangue_display_set_icon(char* s);

int harangue_display_getc(void);

#define KEY_BACKSPACE 127
#define KEY_DC 255
