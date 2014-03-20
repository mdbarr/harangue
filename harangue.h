/***************************************************************************
 *                                                                         *
 *  Harangue IRC Client using VCSI for IRC Protocol / Scripting            *
 *                                                                         *
 ***************************************************************************/

#define HARANGUE_1_0_0
#define HARANGUE_MAJOR_VERSION 1
#define HARANGUE_MINOR_VERSION 0
#define HARANGUE_PATCH_VERSION 0

#define HARANGUE_VERSION HARANGUE_MAJOR_VERSION, HARANGUE_MINOR_VERSION, HARANGUE_PATCH_VERSION

#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

#include <vcsi/vcsi.h>

#include "display.h"
#include "unicode.h"
#include "irc.h"
#include "timer.h"
#include "engine.h"

#include "colors.h"
#define HARANGUE_256COLOR_NICK_COLORS 56

extern char* harangue_name;
extern char* harangue_version;

int harangue_config_timestamps(void);
int harangue_config_hostmasks(void);
int harangue_config_spellcheck(void);
int harangue_config_256color(void);
void harangue_config_bell(void);

void harangue_display_theme_attr(int a, int b);
void harangue_display_theme_color(int a, int b);

void harangue_nick_color(char* nick, int* fg, int* bold);
int harangue_colorize_nick(char* nick);
struct tm* harangue_localtime(void);
int harangue_bold_map(int c);
int harangue_color_map(int c);
int harangue_mirc_color_map(int fg, int b);

int harangue_256color_colorize_mapper(char *nick);
int harangue_256color_colorize_map(int m);
int harangue_256color_nick_color(char* nick);

void harangue_scroll(void);
void harangue_display_text_wrap(char* str, int initial_indent);

void harangue_display_timestamp(void);
int harangue_main_indent(void);
void harangue_display_indent(void);

void harangue_display_rep_in(char* input);
void harangue_display_rep_out(VCSI_CONTEXT vc, char* output);

void harangue_display_public(char* source, char* channel, char* text);
void harangue_display_private(char* source, char* hostmask, char* text);
void harangue_display_action(char* source, char* channel, char* text);
void harangue_display_notice(char* source, char* hostmask, char* text);
void harangue_display_ctcp(char* source, char* hostmask, char* text);

int harangue_display_system_header(void);
void harangue_display_nick_change(char* old, char* new);

void harangue_display_system(char* text);
void harangue_display_system_printf(const char *format, ...);

void harangue_display_generic(char* text);
void harangue_display_generic_printf(const char *format, ...);

void harangue_display_raw(char* text);
void harangue_display_raw_printf(const char *format, ...);

void harangue_display_doubled(char* text);

void harangue_display_message_printf(char* msg, int color16, int color256, const char *format, ...);
#define harangue_display_warning(text) harangue_display_message_printf("Warning", ATTR_FG_YELLOW, THEME_256_WARNING, text);
#define harangue_display_warning_printf(...) harangue_display_message_printf("Warning", ATTR_FG_YELLOW, THEME_256_WARNING, __VA_ARGS__);
#define harangue_display_error(text) harangue_display_message_printf("Error", ATTR_FG_YELLOW, THEME_256_ERROR, text);
#define harangue_display_error_printf(...) harangue_display_message_printf("Error", ATTR_FG_YELLOW, THEME_256_ERROR, __VA_ARGS__);

void harangue_display_chan_msg(char* type, char* channel, char* text);

void harangue_display_public_me(char* text);
void harangue_display_private_me(char* target, char* text);
void harangue_display_notice_me(char* target, char* text);
void harangue_display_action_me(char* text);
void harangue_display_ctcp_me(char* target, char* text);

char* harangue_status_fancy_time(char* buff, int len,
				 char* title, unsigned long time,
				 int disp_zero);
void harangue_display_parsed_string(char *buff);
void harangue_status_display(void);
void harangue_256color_status_display(void);

void harangue_cmdline_print(char *cmd, int remaining);
int harangue_cmdline_prompt_length(void);
int harangue_cmdline_display_prompt(void);
void harangue_cmdline_display(char *cmd, int cursor);


VCSI_OBJECT harangue_read_eval(VCSI_CONTEXT vc, char* s);

VCSI_OBJECT harangue_port_display(VCSI_CONTEXT vc,
				  VCSI_OBJECT args,
				  int length);  

VCSI_OBJECT harangue_display_doubled_vcsi(VCSI_CONTEXT vc,
					 VCSI_OBJECT x);

VCSI_OBJECT harangue_display_public_vcsi(VCSI_CONTEXT vc,
					 VCSI_OBJECT x,
					 VCSI_OBJECT y,
					 VCSI_OBJECT z);
VCSI_OBJECT harangue_display_private_vcsi(VCSI_CONTEXT vc,
					  VCSI_OBJECT x,
					  VCSI_OBJECT y,
					  VCSI_OBJECT z);
VCSI_OBJECT harangue_display_action_vcsi(VCSI_CONTEXT vc,
					 VCSI_OBJECT x,
					 VCSI_OBJECT y,
					 VCSI_OBJECT z);
VCSI_OBJECT harangue_display_notice_vcsi(VCSI_CONTEXT vc,
					 VCSI_OBJECT x,
					 VCSI_OBJECT y,
					 VCSI_OBJECT z);
VCSI_OBJECT harangue_display_nick_change_vcsi(VCSI_CONTEXT vc,
					      VCSI_OBJECT x,
					      VCSI_OBJECT y);
VCSI_OBJECT harangue_display_system_vcsi(VCSI_CONTEXT vc,
					 VCSI_OBJECT x);
VCSI_OBJECT harangue_display_generic_vcsi(VCSI_CONTEXT vc,
					  VCSI_OBJECT x);
VCSI_OBJECT harangue_display_raw_vcsi(VCSI_CONTEXT vc,
				      VCSI_OBJECT x);
VCSI_OBJECT harangue_display_chan_msg_vcsi(VCSI_CONTEXT vc,
					   VCSI_OBJECT x,
					   VCSI_OBJECT y, 
					   VCSI_OBJECT z);
VCSI_OBJECT harangue_display_ctcp_vcsi(VCSI_CONTEXT vc,
				       VCSI_OBJECT x,
				       VCSI_OBJECT y, 
				       VCSI_OBJECT z);
VCSI_OBJECT harangue_string_time_vcsi(VCSI_CONTEXT vc,
				      VCSI_OBJECT x);
VCSI_OBJECT harangue_display_chan_names(VCSI_CONTEXT vc,
					VCSI_OBJECT chan,
					VCSI_OBJECT names);
VCSI_OBJECT harangue_command_parse(VCSI_CONTEXT vc,
				   char *s);
VCSI_OBJECT harangue_command_parse_vcsi(VCSI_CONTEXT vc,
					VCSI_OBJECT x);
VCSI_OBJECT harangue_colorize_nick_vcsi(VCSI_CONTEXT vc,
					VCSI_OBJECT x);

VCSI_OBJECT harangue_window_title_set_vcsi(VCSI_CONTEXT vc,
					   VCSI_OBJECT x);
VCSI_OBJECT harangue_window_icon_set_vcsi(VCSI_CONTEXT vc,
					  VCSI_OBJECT x);

char* harangue_print_object_vcsi(VCSI_CONTEXT vc,
				 INFOSTD_DYN_STR result,
				 VCSI_OBJECT item, 
				 int clear);

void harangue_rep(VCSI_CONTEXT vc, char* input);

void harangue_vcsi_init(VCSI_CONTEXT vc);
void harangue_handler(VCSI_CONTEXT vc);

void harangue_banner(void);

char* harangue_comma_format(INFOSTD_DYN_STR ds);
VCSI_OBJECT harangue_comma_format_vcsi(VCSI_CONTEXT vc,
				       VCSI_OBJECT x);

/* Macros */
#define HARANGUE_256COLORS harangue_engine->colors == 256
#define HARANGUE_COLORIZE_NICK(x) (harangue_engine->colors == 256) ? harangue_256color_colorize_nick(x) : harangue_colorize_nick(x)
#define HARANGUE_STATUS_DISPLAY() (harangue_engine->colors == 256) ? harangue_256color_status_display() : harangue_status_display()

/* Theme/256 Defines */
#define THEME_256_STATUS_BG COLOR_256_DODGERBLUE3
#define THEME_256_TIMESTAMP COLOR_256_DEEPSKYBLUE3
//#define COLOR_256_PUBLIC    COLOR_256_DEEPSKYBLUE3
#define THEME_256_PUBLIC_ME COLOR_256_DODGERBLUE3
//#define THEME_256_PUBLIC    COLOR_256_GREY11
#define THEME_256_PUBLIC    COLOR_256_GREY13
//#define THEME_256_PRIVATE   COLOR_256_MEDIUMTURQUOISE
#define THEME_256_PRIVATE   COLOR_256_PALETURQUOISE
#define THEME_256_NOTICE    COLOR_256_DARKRED
#define THEME_256_CTCP      COLOR_256_MEDIUMORCHID
#define THEME_256_CHANNEL   COLOR_256_SPRINGGREEN
#define THEME_256_COMMAND   COLOR_256_GREY13
//#define COLOR_256_COMMAND   COLOR_256_LIGHTSLATEGREY
#define THEME_256_WARNING   COLOR_256_DARKORANGE
#define THEME_256_ERROR     COLOR_256_RED2
#define THEME_256_OP        COLOR_256_LIGHTCYAN
#define THEME_256_ACTION    COLOR_256_WHITE

//#define COLOR_256_STATUS_BG COLOR_256_DODGERBLUE
//#define COLOR_256_TIMESTAMP COLOR_256_DEEPSKYBLUE

//#define COLOR_256_STATUS_BG COLOR_256_DODGERBLUE3
//#define COLOR_256_TIMESTAMP COLOR_256_DODGERBLUE3

/* Theme/16 Defines */
#define THEME_16_STATUS_BG ATTR_BG_BLUE
#define THEME_16_TIMESTAMP ATTR_FG_BLUE
#define THEME_16_PUBLIC_ME ATTR_FG_CYAN
#define THEME_16_PUBLIC    ATTR_FG_CYAN
#define THEME_16_PRIVATE   ATTR_FG_CYAN
#define THEME_16_NOTICE    ATTR_FG_RED
#define THEME_16_CTCP      ATTR_FG_MAGENTA
#define THEME_16_CHANNEL   ATTR_FG_CYAN
#define THEME_16_COMMAND   ATTR_FG_CYAN
#define THEME_16_WARNING   ATTR_FG_YELLOW
#define THEME_16_ERROR     ATTR_FG_RED
#define THEME_16_OP        ATTR_FG_CYAN
#define THEME_16_ACTION    ATTR_FG_WHITE
