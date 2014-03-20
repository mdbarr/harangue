#include "harangue.h"

int harangue_window_rows = 0;
int harangue_window_cols = 0;
struct termios harangue_window_tty;

/* Setup - 
   Detech Window Size, unbuffered output, set custom tty settings for input */
void harangue_display_setup(void) {

    struct winsize w;
    struct termios tty;

    ioctl(0, TIOCGWINSZ, &w);

    harangue_window_rows = w.ws_row;
    harangue_window_cols = w.ws_col;

    setbuf(stdout, NULL);

    tcgetattr(0, &harangue_window_tty);
    tcgetattr(0, &tty);

    tty.c_cc[ VMIN ] = 0;    
    tty.c_cc[ VTIME ] = 0;
    tty.c_lflag &= ~ECHO;
    tty.c_lflag &= ~ICANON;
    tty.c_lflag &= ~ISIG;
    tcsetattr(0, TCSANOW, &tty);
}

/* Clean Up */
void harangue_display_cleanup(void) {

  harangue_display_scroll_set(1,harangue_window_rows);
  harangue_display_attribute(ATTR_RESET);
  harangue_display_erase_screen();

  tcsetattr(0, TCSANOW, &harangue_window_tty);
}

void harangue_display_reset(void) {
  printf("\ec");
}

/* Cursor Movement */
void harangue_display_cursor_move(int row, int col) {
  printf("\e[%d;%dH",row,col);
}

void harangue_display_cursor_up(int count) {
  printf("\e[%dA",count);
}

void harangue_display_cursor_down(int count) {
  printf("\e[%dB",count);
}

void harangue_display_cursor_forward(int count) {
  printf("\e[%dC",count);
}

void harangue_display_cursor_backward(int count) {
  printf("\e[%dD",count);
}

void harangue_display_cursor_save(void) {
  printf("\e7");
}

void harangue_display_cursor_restore(void) {
  printf("\e8");
}

/* Scrolling */
void harangue_display_scroll_set(int start, int end) {
  printf("\e[%d;%dr",start,end);
}

void harangue_display_scroll_down(void) {
  printf("\eD");
}

void harangue_display_scroll_up(void) {
  printf("\eM");
}

/* Erasing */
void harangue_display_erase_to_end(void) {
  printf("\e[K");
}

void harangue_display_erase_to_start(void) {
  printf("\e[1K");
}

void harangue_display_erase_line(void) {
  printf("\e[2K");
}

void harangue_display_erase_down(void) {
  printf("\e[J");
}

void harangue_display_erase_up(void) {
  printf("\e[1J");
}

void harangue_display_erase_screen(void) {
  printf("\e[2J");
}

/* Attributes and Colors */
void harangue_display_attribute(int attr) {
  printf("\e[%dm",attr);
}

void harangue_display_attributes(int attr, int fore, int back) {
  if(fore < 10)
    fore += 30;
  if(back < 10)
    back += 40;

  printf("\e[%d;%d;%dm", attr, fore, back);
}

void harangue_display_256color_foreground(int color) {
  printf("\e[38;5;%dm", color);
}

void harangue_display_256color_background(int color) {
  printf("\e[48;5;%dm", color);
}

void harangue_display_256color_attributes(int attr, int fore, int back) {
  printf("\e[%d;38;5;%d;48;5;%dm", attr, fore, back);
}

void harangue_display_special(char *s) {
  printf("\e(0%s\e(B", s);
}

void harangue_display_bell(void) {
  printf("\a");
}

// Line Attributes
void harangue_display_line_doubled_top(void) {
  printf("\e#3");
}

void harangue_display_line_doubled_bottom(void) {
  printf("\e#4");
}

void harangue_display_line_double_width(void) {
  printf("\e#6");
}

// Title and Icon setting
void harangue_display_set_title(char* s) {
  printf("\e]2;%s\a", s);
}

void harangue_display_set_icon(char* s) {
  printf("\e]1;%s\a", s);
}

/* Quick, non-blocking read */
int harangue_display_getc(void) {
  char c;

  if(read(0,&c,1) == 1) {
    return (int) c;
  } 
  return -1;
}
