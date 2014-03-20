/***************************************************************************
 *                                                                         *
 *  Harangue IRC Client using VCSI for IRC Protocol / Scripting            *
 *                                                                         *
 ***************************************************************************/

#include "harangue.h"

char* harangue_name = "harangue";
char* harangue_version = "Harangue IRC Client v1.0.0";

/***************************************************************************/

int harangue_config_timestamps(void) {
  return (HARANGUE_OPTION_CHECK(HARANGUE_OPTION_TIMESTAMP)) ? 1 : 0;
}

int harangue_config_hostmasks(void) {
  return (HARANGUE_OPTION_CHECK(HARANGUE_OPTION_HOSTMASK)) ? 1 : 0;
}

int harangue_config_spellcheck(void) {
  if(HARANGUE_OPTION_CHECK(HARANGUE_OPTION_SPELLCHECK) && harangue_engine->speller) {
    return 1;
  }
  return 0;
}

int harangue_config_256color(void) {
  return (HARANGUE_COLORS(256)) ? 1 : 0;
}

void harangue_config_bell(void) {
  if(HARANGUE_OPTION_CHECK(HARANGUE_OPTION_BELL)) {
    harangue_display_bell();
  }
}
  
void harangue_display_theme_attr(int a, int b) {
  if(HARANGUE_COLORS(256)) {
    harangue_display_attribute(b);
  } else {
    harangue_display_attribute(a);
  }
}

void harangue_display_theme_color(int a, int b) {
  if(HARANGUE_COLORS(256)) {
    harangue_display_256color_foreground(b);
  } else {
    harangue_display_attribute((a < 30) ? a + 30 : a);
  }
}

/***************************************************************************/

void harangue_nick_color(char* nick, int* fg, int* bold) {
  unsigned long i, l, c, h;

  l = strlen(nick);
  for(i = 0, c = 0; i < l; i++) {
    //c += (unsigned long)tolower(nick[i]);
    if(nick[i] == '@') continue;
    c += (unsigned long)nick[i];
  }
  
  c = (((c / 7 + l) + 3) % 8) + 2;
  h = (c / 4) % 2;
  c = (c % 4) + 2;
  h = (h == 0 || c == 4) ? 1 : 0;

  *fg = c;
  *bold = h;
}

int harangue_colorize_nick(char* nick) {
  int fg_color, bg_color, bold, len;

  harangue_nick_color(nick,&fg_color,&bold);

  bg_color = COLOR_BLACK;

  harangue_display_attributes(0, fg_color, bg_color);

  if(bold) harangue_display_attribute(ATTR_BOLD);

  len = printf("%s", nick);

  if(bold) harangue_display_attribute(ATTR_NONE);

  harangue_display_attributes(0, COLOR_WHITE, COLOR_BLACK);

  return len;
}

struct tm* harangue_localtime(void) {
  time(&harangue_engine->clock);
  return localtime(&harangue_engine->clock);
}

int harangue_bold_map(int c) {
  switch (c) {
  case 1:
  case 2:
  case 3:
  case 4:
  case 6:
  case 7:
  case 15:
    return 0;
    break;
  case 0:
  case 5:
  case 8:
  case 9:
  case 11:
  case 12:
  case 13:
  case 14:
  case 16:
    return 1;
    break;
  }
  return 0;
}

int harangue_color_map(int c) {
  switch(c) {
  case 0:
  case 15:
  case 16:
    return COLOR_WHITE;
    break;
  case 1:
  case 14:
    return COLOR_BLACK;
    break;
  case 2:
  case 12:
    return COLOR_BLUE;
    break;
  case 3:
  case 9:
    return COLOR_GREEN;
    break;
  case 4:
  case 5:
    return COLOR_RED;
    break;
  case 6:
  case 13:
    return COLOR_MAGENTA;
    break;
  case 7:
  case 8:
    return COLOR_YELLOW;
    break;
  case 10:
  case 11:
    return COLOR_CYAN;
    break;    
  }
  return COLOR_WHITE;
}

int harangue_mirc_color_map(int fg, int b) {
  switch(fg) {
  case COLOR_WHITE:
    return (b) ? 0 : 15;
    break;
  case COLOR_BLACK:
    return (b) ? 1 : 14;
    break;
  case COLOR_BLUE:
    return (b) ? 12 : 2;
    break;
  case COLOR_GREEN:
    return (b) ? 9 : 3;
    break;
  case COLOR_RED:
    return (b) ? 5 : 4;
    break;
  case COLOR_MAGENTA:
    return (b) ? 13 : 6;
    break;
  case COLOR_YELLOW:
    return (b) ? 8 : 7;
    break;
  case COLOR_CYAN:
    return (b) ? 11 : 10;
    break;
  }
  return 0;
}

int harangue_256color_colorize_mapper(char *nick) {
  int l, i, c, hash = 0;

  l = strlen(nick);
  for(i = 0, c = 0; i < l; i++) {
    if(nick[i] == '@' || nick[i] == ' ') continue;
    if(nick[i] == '#') c += 17;
    c += (unsigned long)tolower(nick[i]);
    //c += nick[i];
    hash = ((hash << 11) + hash) + c + 23;
  }
  if(hash < 0) hash *= -1;
  hash %= HARANGUE_256COLOR_NICK_COLORS;
  return hash;
}

int harangue_256color_colorize_map(int m) {
  switch(m) {
  case 0:
    return COLOR_256_DODGERBLUE;
    break;
  case 1:
    return COLOR_256_ROSYBROWN;
    break;
  case 2:
    return COLOR_256_DARKGOLDENROD;
    break;
  case 3:
    return COLOR_256_LIGHTGREEN;
    break;
  case 4:
    return COLOR_256_LIGHTYELLOW;
    break;
  case 5:
    return COLOR_256_SEAGREEN;
    break;
  case 6:
    return COLOR_256_GREENYELLOW;
    break;
  case 7:
    return COLOR_256_SPRINGGREEN;
    break;
  case 8:
    return COLOR_256_CYAN;
    break;
  case 9:
    return COLOR_256_LIGHTSTEELBLUE;
    break;
  case 10:
    return COLOR_256_LIGHTSLATEGREY;
    break;
  case 11:
    return COLOR_256_LIGHTSEAGREEN;
    break;
  case 12:
    return COLOR_256_BLUE;
    break;
  case 13:
    return COLOR_256_DARKORANGE;
    break;
  case 14:
    return COLOR_256_DEEPPINK;
    break;
  case 15:
    return COLOR_256_DARKSLATEGRAY;
    break;
  case 16:
    return COLOR_256_DARKTURQUOISE;
    break;
  case 17:
    return COLOR_256_LIGHTSALMON;
    break;
  case 18:
    return COLOR_256_TURQUOISE;
    break;
  case 19:
    return COLOR_256_LIGHTGOLDENROD;
    break;
  case 20:
    return COLOR_256_GREEN;
    break;
  case 21:
    return COLOR_256_DARKOLIVEGREEN;
    break;
  case 22:
    return COLOR_256_DARKKHAKI;
    break;
  case 23:
    return COLOR_256_GOLD;
    break;
  case 24:
    return COLOR_256_LIGHTPINK;
    break;
  case 25:
    return COLOR_256_SKYBLUE;
    break;
  case 26:
    return COLOR_256_LIGHTSKYBLUE;
    break;
  case 27:
    return COLOR_256_DARKCYAN;
    break;
  case 28:
    return COLOR_256_PURPLE;
    break;
  case 29:
    return COLOR_256_DARKVIOLET;
    break;
  case 30:
    return COLOR_256_MAGENTA;
    break;
  case 31:
    return COLOR_256_STEELBLUE;
    break;
  case 32:
    return COLOR_256_PINK;
    break;
  case 33:
    return COLOR_256_DARKSEAGREEN;
    break;
  case 34:
    return COLOR_256_HOTPINK;
    break;
  case 35:
    return COLOR_256_INDIANRED;
    break;
  case 36:
    return COLOR_256_KHAKI;
    break;
  case 37:
    return COLOR_256_SLATEBLUE;
    break;
  case 38:
    return COLOR_256_DARKMAGENTA;
    break;
  case 39:
    return COLOR_256_ORCHID;
    break;
  case 40:
    return COLOR_256_AQUAMARINE;
    break;
  case 41:
    return COLOR_256_ROYALBLUE;
    break;
  case 42:
    return COLOR_256_ORANGE;
    break;
  case 43:
    return COLOR_256_TAN;
    break;
  case 44:
    return COLOR_256_SALMON;
    break;
  case 45:
    return COLOR_256_SANDYBROWN;
    break;
  case 46:
    return COLOR_256_CHARTREUSE;
    break;
  case 47:
    return COLOR_256_CORNFLOWERBLUE;
    break;
  case 48:
    return COLOR_256_WHEAT;
    break;
  case 49:
    return COLOR_256_DARKGREEN;
    break;
  case 50:
    return COLOR_256_CADETBLUE;
    break;
  case 51:
    return COLOR_256_VIOLET;
    break;
  case 52:
    return COLOR_256_LIGHTCORAL;
    break;
  case 53:
    return COLOR_256_YELLOW;
    break;
  case 54:
    return COLOR_256_BLUEVIOLET;
    break;
  case 55:
    return COLOR_256_DEEPSKYBLUE;
    break;
  }
  return COLOR_256_WHITE;
}

int harangue_256color_nick_color(char* nick) {
  int m;
  m = harangue_256color_colorize_mapper(nick);
  return harangue_256color_colorize_map(m);
}

int harangue_256color_colorize_nick(char* nick) {
  int fg, len;

  fg = harangue_256color_nick_color(nick);

  harangue_display_256color_foreground(fg);

  len = printf("%s", nick);

  harangue_display_attribute(ATTR_NONE);

  return len;
}

void harangue_scroll(void) {
  // Move to the last line of the scroll region
  harangue_display_cursor_move(harangue_engine->line_last, 1);
  // Scroll down one line within the scroll region
  harangue_display_scroll_down();
  // Reposition on the new line at the bottom of the scroll region
  harangue_display_cursor_move(harangue_engine->line_last, 1);
  // Reset the line attributes - Nope!
  //harangue_display_attribute(ATTR_NONE);
  // Erase the line with the blank attribute
  harangue_display_erase_line();
}

void harangue_display_text_wrap(char* str, int initial_indent) {
  char* buff;
  int i, j, len, start, rs, ls, width;
  int bold = 0;
  int in_color = 0, digit;
  int fg_color = 0, bg_color = 1;
  unsigned long uchar;
  int in_unicode = 0;
  int indent = 0;

  len = strlen(str);

  buff = malloc(len + 1);
  memset(buff, 0, len+1);
  strncpy(buff, str, len);

  width = harangue_engine->window_columns - 2;

  indent = harangue_main_indent();

  initial_indent += indent;

  rs = width - initial_indent;

  // Add returns
  for(i = 0, ls = 0, start = 0; i < len; i++) {

    if(buff[i] == '') { 
      continue;
    } else if(buff[i] == '') {
      in_color = 1;
      continue;
    } else if(buff[i] == '') {
      in_unicode = 1;
      continue;
    } else if(in_color) {
      if(isdigit(buff[i])) {
	continue;
      } else if(buff[i] == ',' && in_color < 2) {
	in_color++;
	continue;
      } else if(buff[i] == ';') {
	in_color = 0;
	continue;
      } else
	in_color = 0;
    } else if(in_unicode) {
      if(buff[i] == ';')
	in_unicode = 0; // No continue for length counting
      else
	continue;
    }

    if(buff[i] == ' ') {
      ls = i;
    }       

    if(rs <= 0 && ls != start) {
      buff[ls] = '\n';
      start = ls+1;
      rs = width - indent;
      rs -= i - start;            
    }
    
    rs--;
  }

  // Print
  in_color = 0;
  in_unicode = 0;
  digit = 0;
  uchar = 0;

  for(i = 0; i < len; i++) {    
    if(buff[i] == '\n') {
      harangue_scroll();
      in_color = 0;
      for(j = 0; j < indent; j++) {
	printf(" ");
      }
      continue;
    } else if(buff[i] == '') {
      if(bold) {
	harangue_display_attribute(ATTR_NORMAL);
	bold = 0;
      } else {
	harangue_display_attribute(ATTR_BOLD);
	bold = 1;
      }
      continue;
    } else if(buff[i] == '') {
      in_color = 1;
      fg_color = 0;
      bg_color = 1;
      continue;
    } else if(in_color) {
      if(isdigit(buff[i])) {
	digit = buff[i] - '0';
	if(in_color == 1) fg_color = (fg_color * 10) + digit;
	if(in_color == 2) bg_color = (bg_color * 10) + digit;
	continue;
      } else if(buff[i] == ',' && in_color < 2) {
	in_color++;
	bg_color = 0;
	continue;
      } else {
	in_color = 0;

	if(bold)
	  harangue_display_attribute(ATTR_NORMAL);

	// Only use 256 colors for values greater than 15, to preserve mirc color maps
	if(harangue_engine->colors == 256 && fg_color > 15) {
	  if(bg_color != 1) {
	    harangue_display_256color_background(bg_color);
	  }
	  if(fg_color != 0) {
	    harangue_display_256color_foreground(fg_color);
	  } else {
	    harangue_display_attribute(ATTR_NONE);
	  }
	} else {	  
	  if(fg_color == 0 || fg_color > 16) {
	    harangue_display_attribute(ATTR_NONE);
	  } else {
	    bold = harangue_bold_map(fg_color);	  
	    fg_color = harangue_color_map(fg_color);
	    bg_color = harangue_color_map(bg_color);
	    harangue_display_attributes(bold, fg_color, bg_color);
	  }
	}
	if(buff[i] != ';')
	  i--; // Try to hop back to preprocess this character
	continue;
      }
    } else if(buff[i] == '') {
      in_unicode = 1;
      uchar = 0;
      continue;
    } else if(in_unicode) {
      if(buff[i] == ';') {
	in_unicode = 0;
	if(HARANGUE_OPTION_CHECK(HARANGUE_OPTION_UNICODE))
	  harangue_display_unicode_character(uchar);
      } else {
	if(isdigit(buff[i]))
	  digit = buff[i] - '0';
	if(isalpha(buff[i]))
	  digit = tolower(buff[i]) - 87;

	uchar = (uchar * 16) + digit;	
      }
      continue;
    }
    printf("%c", buff[i]);
  }

  harangue_display_attribute(ATTR_NONE);

  free(buff);
}

/***************************************************************************/

void harangue_display_timestamp(void) {
  struct tm* lt;

  if(harangue_config_timestamps()) {
    
    lt = harangue_localtime();

    if(HARANGUE_256COLORS) {
      harangue_display_256color_foreground(THEME_256_TIMESTAMP);
      printf("[");
      harangue_display_attribute(ATTR_NONE);
    } else {
      harangue_display_attribute(ATTR_BRIGHT);
      harangue_display_attribute(THEME_16_TIMESTAMP);
      printf("[");
      harangue_display_attribute(ATTR_NONE);
      harangue_display_attribute(ATTR_FG_WHITE);
    }
    printf("%2.2d:%2.2d", lt->tm_hour, lt->tm_min);

    if(HARANGUE_256COLORS) {
      harangue_display_256color_foreground(THEME_256_TIMESTAMP);
      printf("] ");
      harangue_display_attribute(ATTR_NONE);
    } else {
      harangue_display_attribute(ATTR_BRIGHT);
      harangue_display_attribute(ATTR_FG_BLUE);
      printf("] ");
      harangue_display_attribute(ATTR_NONE);
    }
  }
}

int harangue_main_indent(void) {
  if(harangue_config_timestamps())
    return 8; // Should be in the header file
  return 0;
}

void harangue_display_indent(void) {
  if(harangue_config_timestamps()) {
    printf("        "); // should be in the header file
  } 
}

void harangue_display_rep_in(char* input) {
  int len = 0;

  harangue_display_cursor_save();

  harangue_scroll();

  harangue_display_timestamp();
  
  harangue_display_attribute(ATTR_BOLD);
  if(HARANGUE_OPTION_CHECK(HARANGUE_OPTION_UNICODE)) {
    harangue_display_unicode_character(UNICODE_LAMBDA);
    len += printf(" ") + 1;
  } else {
    len += printf("]=> ");
  }
  harangue_display_attribute(ATTR_NONE);

  harangue_display_text_wrap(input, len);

  harangue_display_cursor_restore();
}

void harangue_display_rep_out(VCSI_CONTEXT vc, char* output) {
  int len = 0;

  harangue_display_cursor_save();

  harangue_scroll();

  harangue_display_indent();
  if(vc->got_error) {
    harangue_display_theme_color(THEME_16_ERROR, THEME_256_ERROR);
    len += printf("; Error: ");
    harangue_display_attribute(ATTR_NONE);
    
    infostd_dyn_str_printf(vc->tmpstr, "%s. See errobj for more details.", STR(CAR(VCELL(vc->errobj))));
    output = vc->tmpstr->buff;
  }
    
  harangue_display_text_wrap(output, len);

  harangue_display_cursor_restore();
}

void harangue_display_public(char* source, char* channel, char* text) {
  int len = 0;

  harangue_display_cursor_save();
  harangue_scroll();

  harangue_display_timestamp();

  harangue_display_theme_color(THEME_16_PUBLIC, THEME_256_PUBLIC);
  len += printf("<");
  harangue_display_attribute(ATTR_NONE);

  len += HARANGUE_COLORIZE_NICK(source);

  if(strcmp(channel, harangue_engine->ircc->current_chan->buff) != 0) {
    harangue_display_theme_color(THEME_16_PUBLIC, THEME_256_PUBLIC);
    len += printf("/");
    harangue_display_attribute(ATTR_NONE);
    len += HARANGUE_COLORIZE_NICK(channel);
  }

  harangue_display_theme_color(THEME_16_PUBLIC, THEME_256_PUBLIC);
  len += printf("> ");
  harangue_display_attribute(ATTR_NONE);

  harangue_display_text_wrap(text, len);

  harangue_display_cursor_restore();
}

void harangue_display_private(char* source, char* hostmask, char* text) {
  int len = 0;

  harangue_display_cursor_save();

  harangue_scroll();

  harangue_display_timestamp();

  harangue_display_theme_color(THEME_16_PRIVATE, THEME_256_PRIVATE);
  len += printf("[msg/");
  harangue_display_attribute(ATTR_NONE);

  len += HARANGUE_COLORIZE_NICK(source);

  harangue_display_theme_color(THEME_16_PRIVATE, THEME_256_PRIVATE);
  
  if(harangue_config_hostmasks())
    len += printf("(%s)", hostmask);

  len += printf("] ");
  harangue_display_attribute(ATTR_NONE);

  harangue_display_text_wrap(text, len);

  harangue_display_cursor_restore();
}

void harangue_display_action(char* source, char* channel, char* text) {
  int len = 0;

  harangue_display_cursor_save();

  harangue_scroll();

  harangue_display_timestamp();

  harangue_display_theme_color(THEME_16_ACTION, THEME_256_ACTION);
  harangue_display_attribute(ATTR_BOLD);
  len += printf("- ");
  harangue_display_attribute(ATTR_NONE);

  len += HARANGUE_COLORIZE_NICK(source);

  if(strcmp(channel, harangue_engine->ircc->current_chan->buff) != 0) {
    harangue_display_theme_color(THEME_16_CHANNEL, THEME_256_CHANNEL);
    len += printf("/");
    harangue_display_attribute(ATTR_NONE);
    len += HARANGUE_COLORIZE_NICK(channel);
  }

  len += printf(" ");

  harangue_display_text_wrap(text, len);

  harangue_display_cursor_restore();
}

void harangue_display_notice(char* source, char* hostmask, char* text) {
  int len = 0;

  harangue_display_cursor_save();

  harangue_scroll();

  harangue_display_timestamp();

  harangue_display_theme_color(THEME_16_NOTICE, THEME_256_NOTICE);
  len += printf("[notice/");
  harangue_display_attribute(ATTR_NONE);

  len += HARANGUE_COLORIZE_NICK(source);

  harangue_display_theme_color(THEME_16_NOTICE, THEME_256_NOTICE);

  if(harangue_config_hostmasks())
    len += printf("(%s)", hostmask);

  len += printf("] ");
  harangue_display_attribute(ATTR_NONE);

  harangue_display_text_wrap(text, len);

  harangue_display_cursor_restore();
}

void harangue_display_ctcp(char* source, char* hostmask, char* text) {
  int len = 0;

  harangue_display_cursor_save();

  harangue_scroll();

  harangue_display_timestamp();

  harangue_display_theme_color(THEME_16_CTCP, THEME_256_CTCP);
  len += printf("[ctcp/");
  harangue_display_attribute(ATTR_NONE);

  len += HARANGUE_COLORIZE_NICK(source);

  harangue_display_theme_color(THEME_16_CTCP, THEME_256_CTCP);

  if(harangue_config_hostmasks())
    len += printf("(%s)", hostmask);

  len += printf("] ");
  harangue_display_attribute(ATTR_NONE);

  harangue_display_theme_color(THEME_16_CTCP, THEME_256_CTCP);
  harangue_display_text_wrap(text, len);
  harangue_display_attribute(ATTR_NONE);

  harangue_display_cursor_restore();
}

int harangue_display_system_header(void) {
  int len = 0;

  harangue_display_timestamp();

  if(HARANGUE_256COLORS) {
    harangue_display_256color_foreground(235);
    len += printf(":");
    harangue_display_256color_foreground(245);
    len += printf(":");
    harangue_display_256color_foreground(255);
    len += printf(": ");
    harangue_display_attribute(ATTR_NONE);
  } else {
    harangue_display_attribute(ATTR_DIM);
    len += printf(":");
    harangue_display_attribute(ATTR_NONE);
    
    harangue_display_attribute(ATTR_FG_BLACK);
    harangue_display_attribute(ATTR_BOLD);
    len += printf(":");
    harangue_display_attribute(ATTR_NONE);
    
    harangue_display_attribute(ATTR_BOLD);
    len += printf(": ");
    harangue_display_attribute(ATTR_NONE);
  }
  return len;
}

void harangue_display_nick_change(char* old, char* new) {
  harangue_display_cursor_save();

  harangue_scroll();

  harangue_display_system_header();

  HARANGUE_COLORIZE_NICK(old);
  printf(" is now known as ");
  HARANGUE_COLORIZE_NICK(new);

  harangue_display_cursor_restore();
}

void harangue_display_system(char* text) {
  int len = 0;

  harangue_display_cursor_save();

  harangue_scroll();

  len += harangue_display_system_header();

  harangue_display_text_wrap(text, len);
  
  harangue_display_cursor_restore();
}

void harangue_display_system_printf(const char *format, ...) {
  int len = 0;
  char buf[1024];
  va_list ap;

  harangue_display_cursor_save();

  harangue_scroll();

  len += harangue_display_system_header();

  va_start(ap, format);
  vsnprintf(buf, 1024, format, ap);
  va_end(ap);

  harangue_display_text_wrap(buf, len);
  
  harangue_display_cursor_restore();
}

void harangue_display_generic(char* text) {
  harangue_display_cursor_save();

  harangue_scroll();

  harangue_display_timestamp();

  harangue_display_text_wrap(text, 0);

  harangue_display_cursor_restore();
}

void harangue_display_generic_printf(const char *format, ...) {
  char buf[1024];
  va_list ap;

  harangue_display_cursor_save();

  harangue_scroll();

  va_start(ap, format);
  vsnprintf(buf, 1024, format, ap);
  va_end(ap);

  harangue_display_timestamp();

  harangue_display_text_wrap(buf, 0);
  
  harangue_display_cursor_restore();
}

void harangue_display_raw(char* text) {
  harangue_display_cursor_save();

  harangue_scroll();

  printf("%s", text);

  harangue_display_cursor_restore();
}

void harangue_display_raw_printf(const char *format, ...) {
  char buf[1024];
  va_list ap;

  harangue_display_cursor_save();

  harangue_scroll();

  va_start(ap, format);
  vsnprintf(buf, 1024, format, ap);
  va_end(ap);

  printf("%s", buf);
  
  harangue_display_cursor_restore();
}

void harangue_display_doubled(char* text) {

  int i = 0;

  harangue_display_cursor_save();

  harangue_scroll();

  if(harangue_config_timestamps()) {
    harangue_display_timestamp();
    harangue_scroll();
    i = harangue_main_indent() / 2;
  }

  harangue_display_line_doubled_top();
  printf("%*s", i, "");
  harangue_display_parsed_string(text);

  harangue_scroll();
  
  harangue_display_line_doubled_bottom();
  printf("%*s", i, "");
  harangue_display_parsed_string(text);

  harangue_display_cursor_restore();
}

void harangue_display_message_printf(char* msg, int color16, int color256, const char *format, ...) {
  char buf[1024];
  va_list ap;
  int len = 0;

  harangue_display_cursor_save();

  harangue_scroll();

  va_start(ap, format);
  vsnprintf(buf, 1024, format, ap);
  va_end(ap);

  len += harangue_display_system_header();

  harangue_display_theme_color(color16, color256);
  len += printf("%s: ", msg);
  harangue_display_attribute(ATTR_NONE);

  harangue_display_text_wrap(buf, len);
  
  harangue_display_cursor_restore();
}

void harangue_display_chan_msg(char* type, char* channel, char* text) {
  int len = 0;

  harangue_display_cursor_save();

  harangue_scroll();

  len += harangue_display_system_header();

  harangue_display_theme_color(THEME_16_CHANNEL, THEME_256_CHANNEL);
  len += printf("[");
  harangue_display_attribute(ATTR_BOLD);
  len += printf("%s", type);
  harangue_display_attribute(ATTR_NORMAL);
  len += printf("/");

  len += HARANGUE_COLORIZE_NICK(channel);

  harangue_display_theme_color(THEME_16_CHANNEL, THEME_256_CHANNEL);
  len += printf("] ");
  harangue_display_attribute(ATTR_NONE);

  harangue_display_text_wrap(text, len);

  harangue_display_cursor_restore();
}

/***************************************************************************/

void harangue_display_public_me(char* text) {
  int len = 0;

  harangue_display_cursor_save();

  harangue_scroll();

  harangue_display_timestamp();

  harangue_display_theme_color(THEME_16_PUBLIC_ME, THEME_256_PUBLIC_ME);
  len += printf("> ");
  harangue_display_attribute(ATTR_NONE);

  harangue_display_text_wrap(text, len);

  harangue_display_cursor_restore();
}

void harangue_display_private_me(char* target, char* text) {
  int len = 0;

  harangue_display_cursor_save();

  harangue_scroll();

  harangue_display_timestamp();

  harangue_display_theme_color(THEME_16_PRIVATE, THEME_256_PRIVATE);
  len += printf("[msg\\");
  
  harangue_display_attribute(ATTR_BOLD);
  len += printf("%s", target);
  harangue_display_attribute(ATTR_NORMAL);

  len += printf("]-> ");
  harangue_display_attribute(ATTR_NONE);

  harangue_display_text_wrap(text, len);

  harangue_display_cursor_restore();
}

void harangue_display_notice_me(char* target, char* text) {
  int len = 0;

  harangue_display_cursor_save();

  harangue_scroll();

  harangue_display_timestamp();

  harangue_display_theme_color(THEME_16_NOTICE, THEME_256_NOTICE);
  len += printf("[notice\\");
  harangue_display_attribute(ATTR_BOLD);
  len += printf("%s", target);
  harangue_display_attribute(ATTR_NORMAL);
  len += printf("]-> ");
  harangue_display_attribute(ATTR_NONE);

  harangue_display_text_wrap(text, len);

  harangue_display_cursor_restore();
}

void harangue_display_action_me(char* text) {
  int len = 0;

  harangue_display_cursor_save();

  harangue_scroll();

  harangue_display_timestamp();

  harangue_display_attribute(ATTR_BOLD);
  len += printf("- %s ", harangue_engine->ircc->nick->buff);
  harangue_display_attribute(ATTR_NONE);

  harangue_display_text_wrap(text, len);

  harangue_display_cursor_restore();
}

void harangue_display_ctcp_me(char* target, char* text) {
  int len = 0;

  harangue_display_cursor_save();

  harangue_scroll();

  harangue_display_timestamp();

  harangue_display_theme_color(THEME_16_CTCP, THEME_256_CTCP);
  len += printf("[ctcp\\");

  harangue_display_attribute(ATTR_BOLD);
  len += printf("%s", target);
  harangue_display_attribute(ATTR_NORMAL);
  len += printf("]-> ");
  harangue_display_attribute(ATTR_NONE);

  harangue_display_theme_color(THEME_16_CTCP, THEME_256_CTCP);
  harangue_display_text_wrap(text, len);
  harangue_display_attribute(ATTR_NONE);

  harangue_display_cursor_restore();
}

/***************************************************************************/

char* harangue_status_fancy_time(char* buff, int len,
				 char* title, unsigned long time,
				 int disp_zero) {
  
  int d, h, m;

  d = floor(time / 86400);
  time -= d * 86400;

  h = floor(time / 3600);
  time -= h * 3600;

  m = floor(time /  60);
  time -= m * 60;

  memset(buff, 0, len);

  if(d > 0 && h > 0 && m > 0) {
    snprintf(buff, len - 1, "%s %dd %dh %dm", title, d, h, m);
  } else if(h > 0 && m > 0) {
    snprintf(buff, len - 1, "%s %dh %dm", title, h, m);
  } else if(m > 0 || disp_zero) {
    snprintf(buff, len - 1, "%s %dm", title, m);
  } 

  return buff;
}

void harangue_display_parsed_string(char *buff) {
  int bold = 0;
  int in_color = 0, digit;
  int fg_color = 0, bg_color = 1;
  unsigned long uchar;
  int in_unicode = 0;
  int i, len;

  len = strlen(buff);

  in_color = 0;
  in_unicode = 0;
  digit = 0;
  uchar = 0;

  for(i = 0; i < len; i++) {    
    if(buff[i] == '\n') {
      continue;
    } else if(buff[i] == '') {
      if(bold) {
	harangue_display_attribute(ATTR_NORMAL);
	bold = 0;
      } else {
	harangue_display_attribute(ATTR_BOLD);
	bold = 1;
      }
      continue;
    } else if(buff[i] == '') {
      in_color = 1;
      fg_color = 0;
      bg_color = 1;
      continue;
    } else if(in_color) {
      if(isdigit(buff[i])) {
	digit = buff[i] - '0';
	if(in_color == 1) fg_color = (fg_color * 10) + digit;
	if(in_color == 2) bg_color = (bg_color * 10) + digit;
	continue;
      } else if(buff[i] == ',' && in_color < 2) {
	in_color++;
	bg_color = 0;
	continue;
      } else {
	in_color = 0;

	if(bold)
	  harangue_display_attribute(ATTR_NORMAL);
	
	// Only use 256 colors for values greater than 15, to preserve mirc color maps
	if(harangue_engine->colors == 256 && fg_color > 15) {
	  if(bg_color != 1) {
	    harangue_display_256color_background(bg_color);
	  }
	  if(fg_color != 0) {
	    harangue_display_256color_foreground(fg_color);
	  } else {
	    harangue_display_attribute(ATTR_NONE);
	  }
	} else {	  
	  if(fg_color == 0 || fg_color > 16) {
	    harangue_display_attribute(ATTR_NONE);
	  } else {
	    bold = harangue_bold_map(fg_color);	  
	    fg_color = harangue_color_map(fg_color);
	    bg_color = harangue_color_map(bg_color);
	    harangue_display_attributes(bold, fg_color, bg_color);
	  }
	}
	if(buff[i] != ';')
	  i--; // Try to hop back to preprocess this character
	continue;
      }
    } else if(buff[i] == '') {
      in_unicode = 1;
      uchar = 0;
      continue;
    } else if(in_unicode) {
      if(buff[i] == ';') {
	in_unicode = 0;
	if(HARANGUE_OPTION_CHECK(HARANGUE_OPTION_UNICODE))
	  harangue_display_unicode_character(uchar);
      } else {
	if(isdigit(buff[i]))
	  digit = buff[i] - '0';
	if(isalpha(buff[i]))
	  digit = tolower(buff[i]) - 87;

	uchar = (uchar * 16) + digit;	
      }
      continue;
    }
    printf("%c", buff[i]);
  }
}

void harangue_status_display(void) {
  unsigned long i, t;
  struct tm* lt;
  char* chan;
  char tmp[255];
  time_t clock;
  char *st1, *st2, *st3, *st4, *st5;

  chan = harangue_engine->ircc->current_chan->buff;
  lt = harangue_localtime();

  st1 = harangue_engine->status1->buff;
  st2 = harangue_engine->status2->buff;
  st3 = harangue_engine->status3->buff;
  st4 = harangue_engine->status4->buff;
  st5 = harangue_engine->status5->buff;

  harangue_display_cursor_save();

  // Side and Nick
  harangue_display_cursor_move(harangue_engine->line_status1, 1);
  harangue_display_attributes(ATTR_BRIGHT, COLOR_WHITE, COLOR_BLUE);
  harangue_display_erase_line();
  
  harangue_display_attributes(ATTR_BRIGHT, COLOR_BLACK, COLOR_BLUE);
  printf("  - ");
  harangue_display_attribute(ATTR_FG_WHITE);
  harangue_display_attributes(ATTR_NONE, COLOR_WHITE, COLOR_BLUE);
  printf("%s", harangue_engine->ircc->nick->buff); //harangue_config_nick());
  harangue_display_attributes(ATTR_BRIGHT, COLOR_BLACK, COLOR_BLUE);
  harangue_display_attribute(ATTR_FG_BLACK);
  printf(" -  ");

  // Status 1
  harangue_display_attributes(ATTR_NONE, COLOR_WHITE, COLOR_BLUE);
  harangue_display_parsed_string(st1);

  // Status 2
  i = (harangue_engine->window_columns - strlen(st2)) / 2;
  harangue_display_cursor_move(harangue_engine->line_status1, i);
  harangue_display_parsed_string(st2);

  // Status 3
  i = harangue_engine->window_columns - (strlen(chan) + strlen(st3) + 6);
  harangue_display_cursor_move(harangue_engine->line_status1, i);
  harangue_display_parsed_string(st3);

  // Channel
  if(strlen(chan)) {
    harangue_display_cursor_move(harangue_engine->line_status1, harangue_engine->window_columns-(strlen(chan) + 3));
    harangue_display_attributes(ATTR_BRIGHT, COLOR_BLACK, COLOR_BLUE);
    printf(" %c", chan[0]);
    harangue_display_attributes(ATTR_NONE, COLOR_WHITE, COLOR_BLUE);
    printf("%s", chan + 1);
  }

  /////////////////////////////////////
  // Second Line

  harangue_display_cursor_move(harangue_engine->line_status2, 1);
  harangue_display_attributes(ATTR_BRIGHT, COLOR_WHITE, COLOR_BLUE);
  harangue_display_erase_line();

  harangue_display_attributes(ATTR_BRIGHT, COLOR_CYAN, COLOR_BLUE);
  printf("  [");
  harangue_display_attributes(ATTR_NONE, COLOR_WHITE, COLOR_BLUE);

  harangue_display_attribute(ATTR_BOLD);
  printf("%s", harangue_name);

  harangue_display_attributes(ATTR_BRIGHT, COLOR_CYAN, COLOR_BLUE);
  printf("]");
  harangue_display_attributes(ATTR_NONE, COLOR_WHITE, COLOR_BLUE);

  // Idle or Away display
  if(harangue_engine->ircc) {
    t = time(&clock);
    
    if(harangue_engine->ircc->away > 0) {
      printf("%s", harangue_status_fancy_time(tmp, 255, " away", t - harangue_engine->ircc->away, 1));
    } else if(harangue_engine->ircc->idle > 0)  {
      printf("%s", harangue_status_fancy_time(tmp, 255, " idle", t - harangue_engine->ircc->idle, 0));
    }
  }

  // Status 4
  harangue_display_attributes(ATTR_NONE, COLOR_WHITE, COLOR_BLUE);
  i = (harangue_engine->window_columns - strlen(st4)) / 2;
  harangue_display_cursor_move(harangue_engine->line_status2, i);
  harangue_display_parsed_string(st4);

  // Status 5
  i = harangue_engine->window_columns - (strlen(st5) + 15);
  harangue_display_cursor_move(harangue_engine->line_status2, i);
  harangue_display_parsed_string(st5);

  // Time and Side
  harangue_display_cursor_move(harangue_engine->line_status2, harangue_engine->window_columns - 12);

  harangue_display_attributes(ATTR_BRIGHT, COLOR_CYAN, COLOR_BLUE);
  printf("  [");
  harangue_display_attributes(ATTR_NONE, COLOR_WHITE, COLOR_BLUE);

  harangue_display_attribute(ATTR_BOLD);
  printf("%2.2d:%2.2d",(lt->tm_hour > 12) ? lt->tm_hour - 12 : lt->tm_hour, lt->tm_min);
  printf("%s", (lt->tm_hour >= 12) ? "PM" : "AM");
  harangue_display_attributes(ATTR_NONE, COLOR_WHITE, COLOR_BLUE);

  harangue_display_attributes(ATTR_BRIGHT, COLOR_CYAN, COLOR_BLUE);
  printf("]  ");

  harangue_display_cursor_restore();
}

void harangue_256color_status_display(void) {
  unsigned long i, t;
  struct tm* lt;
  char* chan;
  char tmp[255];
  time_t clock;
  char *st1, *st2, *st3, *st4, *st5;

  chan = harangue_engine->ircc->current_chan->buff;
  lt = harangue_localtime();

  st1 = harangue_engine->status1->buff;
  st2 = harangue_engine->status2->buff;
  st3 = harangue_engine->status3->buff;
  st4 = harangue_engine->status4->buff;
  st5 = harangue_engine->status5->buff;

  harangue_display_cursor_save();

  // Side and Nick
  harangue_display_cursor_move(harangue_engine->line_status1, 1);
  harangue_display_256color_attributes(ATTR_NORMAL, COLOR_256_WHITE, THEME_256_STATUS_BG);
  harangue_display_erase_line();

  harangue_display_256color_attributes(ATTR_BOLD, COLOR_256_BLACK, THEME_256_STATUS_BG);
  if(HARANGUE_OPTION_CHECK(HARANGUE_OPTION_UNICODE)) {
    printf("  ");
    harangue_display_unicode_character(UNICODE_DOUBLE_ANGLE_LEFT);
    printf(" ");
  } else {
    harangue_display_special("  ~ ");
  }
  harangue_display_256color_attributes(ATTR_NORMAL, COLOR_256_WHITE, THEME_256_STATUS_BG);
  printf("%s", harangue_engine->ircc->nick->buff); 
  harangue_display_256color_attributes(ATTR_BOLD, COLOR_256_BLACK, THEME_256_STATUS_BG);
  if(HARANGUE_OPTION_CHECK(HARANGUE_OPTION_UNICODE)) {
    printf(" ");
    harangue_display_unicode_character(UNICODE_DOUBLE_ANGLE_RIGHT);
    printf("  ");
  } else {
    harangue_display_special(" ~  ");
  }

  // Status 1
  harangue_display_256color_attributes(ATTR_NORMAL, COLOR_256_WHITE, THEME_256_STATUS_BG);
  harangue_display_parsed_string(st1);

  // Status 2
  i = (harangue_engine->window_columns - strlen(st2)) / 2;
  harangue_display_cursor_move(harangue_engine->line_status1, i);
  harangue_display_parsed_string(st2);

  // Status 3
  i = harangue_engine->window_columns - (strlen(chan) + strlen(st3) + 6);
  harangue_display_cursor_move(harangue_engine->line_status1, i);
  harangue_display_parsed_string(st3);

  // Channel
  if(strlen(chan)) {
    harangue_display_cursor_move(harangue_engine->line_status1, harangue_engine->window_columns-(strlen(chan) + 3));
    harangue_display_256color_attributes(ATTR_BOLD, COLOR_256_BLACK, THEME_256_STATUS_BG);
    printf(" %c", chan[0]);
    harangue_display_256color_attributes(ATTR_NORMAL, COLOR_256_WHITE, THEME_256_STATUS_BG);
    printf("%s", chan + 1);
  }

  /////////////////////////////////////
  // Second Line

  harangue_display_cursor_move(harangue_engine->line_status2, 1);
  harangue_display_256color_attributes(ATTR_NORMAL, COLOR_256_WHITE, THEME_256_STATUS_BG);
  harangue_display_erase_line();

  harangue_display_256color_attributes(ATTR_BOLD, COLOR_256_BLACK, THEME_256_STATUS_BG);
  printf("  [");

  harangue_display_256color_attributes(ATTR_BOLD, COLOR_256_WHITE, THEME_256_STATUS_BG);
  printf("%s", harangue_name);

  harangue_display_256color_attributes(ATTR_BOLD, COLOR_256_BLACK, THEME_256_STATUS_BG);
  printf("]");
  harangue_display_256color_attributes(ATTR_NORMAL, COLOR_256_WHITE, THEME_256_STATUS_BG);

  // Idle or Away display
  if(harangue_engine->ircc) {
    t = time(&clock);
    
    if(harangue_engine->ircc->away > 0) {
      printf("%s", harangue_status_fancy_time(tmp, 255, " away", t - harangue_engine->ircc->away, 1));
    } else if(harangue_engine->ircc->idle > 0)  {
      printf("%s", harangue_status_fancy_time(tmp, 255, " idle", t - harangue_engine->ircc->idle, 0));
    }
  }

  // Status 4
  harangue_display_256color_attributes(ATTR_NORMAL, COLOR_256_WHITE, THEME_256_STATUS_BG);
  i = (harangue_engine->window_columns - strlen(st4)) / 2;
  harangue_display_cursor_move(harangue_engine->line_status2, i);
  harangue_display_parsed_string(st4);

  // Status 5
  i = harangue_engine->window_columns - (strlen(st5) + 15);
  harangue_display_cursor_move(harangue_engine->line_status2, i);
  harangue_display_parsed_string(st5);

  // Time and Side
  harangue_display_cursor_move(harangue_engine->line_status2, harangue_engine->window_columns - 12);

  harangue_display_256color_attributes(ATTR_BOLD, COLOR_256_BLACK, THEME_256_STATUS_BG);
  printf("  [");
  harangue_display_256color_attributes(ATTR_NORMAL, COLOR_256_WHITE, THEME_256_STATUS_BG);
  printf("%2.2d:%2.2d",(lt->tm_hour > 12) ? lt->tm_hour - 12 : lt->tm_hour, lt->tm_min);
  printf("%s", (lt->tm_hour >= 12) ? "PM" : "AM");
  harangue_display_256color_attributes(ATTR_BOLD, COLOR_256_BLACK, THEME_256_STATUS_BG);
  printf("]  ");

  harangue_display_cursor_restore();
}

void harangue_cmdline_print(char *cmd, int remaining) {
  int start, len, i, d;
  int msg_last = 0;
  char buf[1024];
  
  len = strlen(cmd);
  for(i = 0, start = 0; i <= len && i < remaining; i++) {
    if(!isalpha(cmd[i]) && cmd[i] != '/' && cmd[i] != '\'' && cmd[i] != 0) {
      d = i - start;
      if(d > 0) {
	memset(buf, 0, 1024);
	strncpy(buf, cmd + start, d);
	if(msg_last) { // Nick with Privmsg to colorize
	  HARANGUE_COLORIZE_NICK(buf);
	  msg_last = 0;
	} else if(d > 2 && cmd[0] != '(' && harangue_config_spellcheck()) {
	  if(buf[0] == '/') { // Command
	    if(HARANGUE_256COLORS) {
	      harangue_display_256color_foreground(THEME_256_COMMAND);
	    } else {
	      harangue_display_attribute(THEME_16_COMMAND);
	    }
	    printf("%s", buf);
	    harangue_display_attribute(ATTR_NONE);
	    if(strncasecmp(buf, "/msg", 4) == 0) {
	      msg_last = 1;
	    }
	  } else if(infostd_speller_check(harangue_engine->speller, (unsigned char*)buf)) {
	    printf("%s", buf);		
	  } else { // Incorrect Spelling
	    if(HARANGUE_OPTION_CHECK(HARANGUE_OPTION_UNICODE_TRICKS)) { // Underline with Unicode
	      harangue_display_unicode_underline(buf, THEME_256_ERROR);
	    } else {
	      if(HARANGUE_256COLORS) { // Color + Underline with ANSI colors
		harangue_display_256color_foreground(THEME_256_ERROR);
		harangue_display_attribute(ATTR_UNDERLINE);
	      } else {
		harangue_display_attribute(ATTR_BRIGHT);
		harangue_display_attribute(THEME_16_ERROR);
	      }
	      printf("%s", buf);
	      harangue_display_attribute(ATTR_NONE);
	    }
	  }	
	} else {
	  printf("%s", buf);
	}
      }
      
      // Check for control character?
      start = i + 1;
      printf("%c", cmd[i]);
    }  
  }
  for(i = start; i <= len && i < remaining; i++)
    printf("%c", cmd[i]);
}

int harangue_cmdline_prompt_length(void) {
  switch(harangue_engine->mode) {
  case MODE_SCHEME:
    if(HARANGUE_OPTION_CHECK(HARANGUE_OPTION_UNICODE))
      return 2; // Lambda
    return 4; // ]=>
    break;
  case MODE_MSG:
    return harangue_engine->msg_mode_nick->length + 3; // [nick] 
    break;
  default:
    return 2;  // >
    break;
  }
}

int harangue_cmdline_display_prompt(void) {
  int len = 0;

  switch(harangue_engine->mode) {
  case MODE_IRC:
    harangue_display_attribute(ATTR_BOLD);
    len = printf("> ");
    harangue_display_attribute(ATTR_NORMAL);
    break;
  case MODE_SCHEME:
    harangue_display_attribute(ATTR_BOLD);
    if(HARANGUE_OPTION_CHECK(HARANGUE_OPTION_UNICODE)) {
      harangue_display_unicode_character(UNICODE_LAMBDA);
      len = printf(" ") + 1;
    } else {
      len = printf("]=> ");
    }
    harangue_display_attribute(ATTR_NORMAL);
    break;
  case MODE_MSG:
    harangue_display_attribute(ATTR_BOLD);
    harangue_display_theme_color(THEME_16_PRIVATE, THEME_256_PRIVATE);
    len = printf("[");
    harangue_display_attribute(ATTR_NORMAL);
    len += HARANGUE_COLORIZE_NICK(harangue_engine->msg_mode_nick->buff);
    harangue_display_attribute(ATTR_BOLD);
    harangue_display_theme_color(THEME_16_PRIVATE, THEME_256_PRIVATE);
    len += printf("] ");
    harangue_display_attribute(ATTR_NONE);
    break;
  }
  return len;
}

void harangue_cmdline_display(char *cmd, int cursor) {
  int diff, p;
  char* s;

  s = cmd;
  p = harangue_cmdline_prompt_length();

  // Line too long for normal display
  if(cursor >= (harangue_engine->window_columns - p)) {
    harangue_display_cursor_move(harangue_engine->line_cmd, 1);
    harangue_display_erase_line();

    diff = ((int)(cursor / (harangue_engine->window_columns - 10)) * (harangue_engine->window_columns - 10)) - 10;
    s+=diff;
    cursor -= diff;

    if(harangue_engine->mode == MODE_SCHEME) {
      printf("%s", s);
    } else {
      harangue_cmdline_print(s, harangue_engine->window_columns - p);
    }

    harangue_display_cursor_move(harangue_engine->line_cmd, cursor+1);

    return;
  }
  //cursor += harangue_cmdline_display_prompt();
  //   harangue_display_cursor_move(harangue_engine->line_cmd, cursor+1);


  // Handle the state
  if(HARANGUE_CMD_STATE_CHECK(HARANGUE_CMD_STATE_ENTERED)) {
    harangue_display_cursor_move(harangue_engine->line_cmd, 1);
    harangue_display_erase_line();
    cursor += harangue_cmdline_display_prompt();
    harangue_display_cursor_move(harangue_engine->line_cmd, cursor+1);
  } else if(HARANGUE_CMD_STATE_CHECK(HARANGUE_CMD_STATE_DELETION_END)) {
    harangue_display_cursor_backward(1);
    harangue_display_erase_to_end();
  } else if(HARANGUE_CMD_STATE_CHECK(HARANGUE_CMD_STATE_CURSOR_MOVE)) {
    cursor += p;
    harangue_display_cursor_move(harangue_engine->line_cmd, cursor+1);
  } else if(HARANGUE_CMD_STATE_CHECK(HARANGUE_CMD_STATE_CHARACTER_APPEND) &&
	    harangue_engine->cmdline->buff[harangue_engine->cmdline->length - 1] != ' ') {
    cursor += p;
    printf("%c", harangue_engine->cmdline->buff[harangue_engine->cmdline->length - 1]);
    harangue_display_cursor_move(harangue_engine->line_cmd, cursor + 1);
  } else {
    // Redraw the whole thing
    harangue_display_cursor_move(harangue_engine->line_cmd, 1);
    harangue_display_erase_line();

    cursor += harangue_cmdline_display_prompt();

    if(harangue_engine->mode == MODE_SCHEME) {
      printf("%s", s);
    } else {
      harangue_cmdline_print(s, harangue_engine->window_columns - p);
    }

    harangue_display_cursor_move(harangue_engine->line_cmd, cursor + 1);
  }

}

void harangue_old_cmdline_display(char* cmd, int cursor) {  
  int diff, p;
  char* s;

  p = (harangue_engine->mode == MODE_SCHEME) ? 4 : 2;

  harangue_display_cursor_move(harangue_engine->line_cmd, 1);
  harangue_display_erase_line();

  if(cursor >= (harangue_engine->window_columns - p)) {
    s = cmd;
    diff = ((int)(cursor / (harangue_engine->window_columns - 10)) * (harangue_engine->window_columns - 10)) - 10;
    s+=diff;

    printf("%s", s);
    cursor -= diff; 
  } else if(harangue_engine->mode == MODE_SCHEME) {
    harangue_display_attribute(ATTR_BOLD);
    printf("]=> ");
    harangue_display_attribute(ATTR_NONE);
    printf("%s", cmd);
    cursor += p;
  } else {
    harangue_display_attribute(ATTR_BOLD);
    printf("> ");
    harangue_display_attribute(ATTR_NONE);
    printf("%s", cmd);
    cursor += p;
  }
  harangue_display_cursor_move(harangue_engine->line_cmd, cursor+1);
}

VCSI_OBJECT harangue_read_eval(VCSI_CONTEXT vc, char* s) {
  VCSI_OBJECT res;

  res = parse_text(vc, s);
  res = eval(vc, res, DYNROOT(vc->root_wind));
  return res;
}

VCSI_OBJECT harangue_port_display(VCSI_CONTEXT vc,
				  VCSI_OBJECT args,
				  int length) {  
  VCSI_OBJECT tmp, x;
  char *s;
  
  if(length > 2)
    return error(vc, "too many arguments to display", args);
  
  x = car(vc, args);
  
  if(length == 2) {
    tmp = cadr(vc, args);
    
    if(!TYPEP(tmp, PORTO) && !TYPEP(tmp, PORTB))
      return error(vc, "object passed to display is the wrong type", tmp);
    
    if(PORTFP(tmp) == NULL)
      return error(vc, "closed port", tmp);
    
    if(TYPEP(x, STRING))
      fprintf(PORTFP(tmp), "%s", STR(x));
    else if(TYPEP(x, CHAR))
      fprintf(PORTFP(tmp), "%c", CHAR(x));
    else
      fprintf(PORTFP(tmp), "%s", print_obj(vc, vc->tmpstr, x, 1));
    
    fflush(PORTFP(tmp));
  } else { // Display as generic text
    if(TYPEP(x, STRING)) {
      harangue_display_generic(STR(x));
    } else {
      s = harangue_print_object_vcsi(vc, vc->tmpstr, x, 1);
      harangue_display_generic(s);
    }
  }
  return vc->novalue;
}

VCSI_OBJECT harangue_display_doubled_vcsi(VCSI_CONTEXT vc,
					  VCSI_OBJECT x) {
  char *s;

  if(TYPEP(x, STRING)) {
    harangue_display_doubled(STR(x));
  } else {
    s = harangue_print_object_vcsi(vc, vc->tmpstr, x, 1);
    harangue_display_doubled(s);
  }
  return vc->novalue;
}
    
VCSI_OBJECT harangue_display_public_vcsi(VCSI_CONTEXT vc,
					 VCSI_OBJECT x,
					 VCSI_OBJECT y,
					 VCSI_OBJECT z) {
  check_arg_type(vc, x, STRING, "harangue-display-public");
  check_arg_type(vc, y, STRING, "harangue-display-public");
  check_arg_type(vc, z, STRING, "harangue-display-public");

  harangue_display_public(STR(x), STR(y), STR(z));

  return vc->novalue;
}

VCSI_OBJECT harangue_display_private_vcsi(VCSI_CONTEXT vc,
					  VCSI_OBJECT x,
					  VCSI_OBJECT y,
					  VCSI_OBJECT z) {
  check_arg_type(vc, x, STRING, "harangue-display-private");
  check_arg_type(vc, y, STRING, "harangue-display-private");
  check_arg_type(vc, z, STRING, "harangue-display-private");
  
  harangue_display_private(STR(x), STR(y), STR(z));

  return vc->novalue;
}

VCSI_OBJECT harangue_display_action_vcsi(VCSI_CONTEXT vc,
					 VCSI_OBJECT x,
					 VCSI_OBJECT y,
					 VCSI_OBJECT z) {
  check_arg_type(vc, x, STRING, "harangue-display-action");
  check_arg_type(vc, y, STRING, "harangue-display-action");
  check_arg_type(vc, z, STRING, "harangue-display-action");

  harangue_display_action(STR(x), STR(y), STR(z));

  return vc->novalue;
}

VCSI_OBJECT harangue_display_notice_vcsi(VCSI_CONTEXT vc,
					 VCSI_OBJECT x,
					 VCSI_OBJECT y,
					 VCSI_OBJECT z) {
  check_arg_type(vc, x, STRING, "harangue-display-notice");
  check_arg_type(vc, y, STRING, "harangue-display-notice");
  check_arg_type(vc, z, STRING, "harangue-display-notice");
  
  harangue_display_notice(STR(x), STR(y), STR(z));

  return vc->novalue;
}

VCSI_OBJECT harangue_display_nick_change_vcsi(VCSI_CONTEXT vc,
					      VCSI_OBJECT x,
					      VCSI_OBJECT y) {
  check_arg_type(vc, x, STRING, "harangue-display-nick-change");
  
  harangue_display_nick_change(STR(x), STR(y));

  return vc->novalue;
}

VCSI_OBJECT harangue_display_system_vcsi(VCSI_CONTEXT vc,
					 VCSI_OBJECT x) {
  check_arg_type(vc, x, STRING, "harangue-display-system");
  
  harangue_display_system(STR(x));

  return vc->novalue;
}

VCSI_OBJECT harangue_display_generic_vcsi(VCSI_CONTEXT vc,
					  VCSI_OBJECT x) {
  check_arg_type(vc, x, STRING, "harangue-display-generic");
  
  harangue_display_generic(STR(x));

  return vc->novalue;
}

VCSI_OBJECT harangue_display_raw_vcsi(VCSI_CONTEXT vc,
				      VCSI_OBJECT x) {
  check_arg_type(vc, x, STRING, "harangue-display-raw");
  
  harangue_display_raw(STR(x));

  return vc->novalue;
}

VCSI_OBJECT harangue_display_chan_msg_vcsi(VCSI_CONTEXT vc,
					   VCSI_OBJECT x,
					   VCSI_OBJECT y, 
					   VCSI_OBJECT z) {
  check_arg_type(vc, x, STRING, "harangue-display-channel-message");
  check_arg_type(vc, y, STRING, "harangue-display-channel-message");
  check_arg_type(vc, z, STRING, "harangue-display-channel-message");
  
  harangue_display_chan_msg(STR(x), STR(y), STR(z));

  return vc->novalue;
}

VCSI_OBJECT harangue_display_ctcp_vcsi(VCSI_CONTEXT vc,
				       VCSI_OBJECT x,
				       VCSI_OBJECT y, 
				       VCSI_OBJECT z) {
  check_arg_type(vc, x, STRING, "harangue-display-ctcp");
  check_arg_type(vc, y, STRING, "harangue-display-ctcp");
  check_arg_type(vc, z, STRING, "harangue-display-ctcp");
  
  harangue_display_ctcp(STR(x), STR(y), STR(z));

  return vc->novalue;
}

VCSI_OBJECT harangue_string_time_vcsi(VCSI_CONTEXT vc,
				      VCSI_OBJECT x) {
  char buff[128];
  char *ln;
  
  check_arg_type(vc, x, LNGNUM, "harangue-string-time");
  
  memset(buff, 0, 128);

  ln = harangue_status_fancy_time(buff, 128, "", LNGN(x), 1);

  return make_raw_string(vc, ln+1);
}

VCSI_OBJECT harangue_display_chan_names(VCSI_CONTEXT vc,
					VCSI_OBJECT chan,
					VCSI_OBJECT names) {
  VCSI_OBJECT n;
  int indent = 5, offset = 0, count = 0;

  check_arg_type(vc, chan, STRING, "harangue-display-channel-names");

  if(harangue_config_timestamps())
    indent += 8;

  harangue_display_cursor_save();

  harangue_scroll();
  
  harangue_display_system_header();
  printf("Users on ");
  HARANGUE_COLORIZE_NICK(STR(chan));
  harangue_display_attribute(ATTR_NONE);
  printf(":");

  harangue_scroll();

  while(names != NULL) {
    n = CAR(names);
    names = CDR(names);
      
    check_arg_type(vc, n, STRING, "harangue-display-channel-names");

    if(STR(n)[0] == '@') {
      harangue_display_cursor_move(harangue_engine->line_last, (indent + offset) - 1);
      harangue_display_theme_color(THEME_16_OP, THEME_256_OP);
      printf("@");
      harangue_display_attribute(ATTR_NONE);
      HARANGUE_COLORIZE_NICK(STR(n)+1);
    } else {
      harangue_display_cursor_move(harangue_engine->line_last, indent + offset);
      HARANGUE_COLORIZE_NICK(STR(n));
    }
    
    offset += 16;
    count++;
    
    if(count % 4 == 0 && names != NULL) {
      offset = 0;
      harangue_scroll();
    }
  }

  harangue_display_cursor_restore();

  return vc->novalue;
}

VCSI_OBJECT harangue_command_parse(VCSI_CONTEXT vc,
				   char *s) {
  VCSI_OBJECT chan = NULL;
  VCSI_OBJECT hirc;
  char *ln, *ln2;
  char str[1024];
  int real_chan = 0;
  int i, len;


  hirc = harangue_engine->irc;

  if(!same_type_user_def(vc, hirc, irc_conn_type))
    error(vc, "no registered irc connection for harangue-command-prase", hirc);

  if(harangue_engine->ircc->current_chan->length) {
    chan = make_string(vc, harangue_engine->ircc->current_chan->buff);
    if(harangue_engine->ircc->current_chan->buff[0] == '#')
      real_chan = 1;
  }
  
  memset(str, 0, 1024);
  strncpy(str, s, 1023);
  len = strlen(str);

  // Clean trailing spaces
  for(i = len - 1; i >= 0; i--) {
    if(str[i] == ' ') {
      str[i] = 0;
      len--;
    } else {
      break;
    }
  }

  // Return if the string is now empty
  if(!len) {
    return vc->novalue;
  }
  
  // Parse the content
  if(!strcasecmp(str, "/connect")) {
    return connect_irc_conn(vc, hirc);
  } else if(!strcasecmp(str, "/disconnect")) {
    return disconnect_irc_conn(vc, hirc);
  } else if(!strncasecmp(str, "/join ", 6) && strlen(str) > 6) {
    ln = str + 6;
    return join_irc_conn(vc, hirc, make_raw_string(vc, ln));
  } else if(!strncasecmp(str, "/part ", 6) && strlen(str) > 6) {
    ln = str + 6;
    return part_irc_conn(vc, hirc, make_raw_string(vc, ln));
  } else if(!strcasecmp(str, "/part") && chan) {
    return part_irc_conn(vc, hirc, chan);
  } else if(!strncasecmp(str, "/nick ", 6) && strlen(str) > 6) {
    ln = str + 6;
    return nick_irc_conn(vc, hirc, make_raw_string(vc, ln));
  } else if(!strncasecmp(str, "/msg ", 5) && strlen(str) > 5) {
    ln = str + 5;
    if((ln2=strchr(ln,' '))) {
      *ln2='\0';
      ln2++;
      harangue_display_private_me(ln, ln2);
      return privmsg_irc_conn(vc, hirc, make_raw_string(vc, ln), make_raw_string(vc, ln2));
    }
  } else if(!strncasecmp(str, "/msgmode ", 9) && strlen(str) > 9) {
    ln = str + 9;
    //harangue_display_generic_printf("attempted msg mode -%s-", ln);
    infostd_dyn_str_strcpy(harangue_engine->msg_mode_nick, ln);
    harangue_engine->mode = MODE_MSG;
    HARANGUE_CMD_STATE_SET(HARANGUE_CMD_STATE_MODE_CHANGE);
    return vc->true;
  } else if(!strncasecmp(str, "/ctcp ", 6) && strlen(str) > 6) {
    ln = str + 6;
    if((ln2=strchr(ln,' '))) {
      *ln2='\0';
      ln2++;
      harangue_display_ctcp_me(ln, ln2);
      return ctcp_irc_conn(vc, hirc, make_raw_string(vc, ln), make_raw_string(vc, ln2));
    }
  } else if(!strncasecmp(str, "/me ", 4)  && strlen(str) > 4 && chan) {
    ln = str + 4;
    harangue_display_action_me(ln);
    return me_irc_conn(vc, hirc, chan, make_raw_string(vc, ln));
  } else if(!strncasecmp(str, "/notice ", 8)  && strlen(str) > 8 && chan) {
    ln = str + 8;
    if((ln2=strchr(ln,' '))) {
      *ln2='\0';
      ln2++;
      harangue_display_notice_me(ln, ln2);
      return notice_irc_conn(vc, hirc, make_raw_string(vc, ln), make_raw_string(vc, ln2));
    }
  } else if(!strncasecmp(str, "/raw ", 5) && strlen(str) > 5) {
    ln = str + 5;
    return raw_irc_conn(vc, hirc, make_raw_string(vc, ln));
  } else if(!strcasecmp(str, "/names")) {
    if(chan)
      names_irc_conn(vc, make_list(vc, 2, hirc, chan), 2);
    else
      names_irc_conn(vc, cons(vc, chan, NULL), 1);
  } else if(!strncasecmp(str, "/ison ", 6) && strlen(str) > 6) {
    ln = str + 6;
    return ison_irc_conn(vc, hirc, make_raw_string(vc, ln));
  } else if(!strncasecmp(str, "/away ", 6) && strlen(str) > 6) {
    ln = str + 6;
    return away_irc_conn(vc, make_list(vc, 2, hirc, make_raw_string(vc, ln)), 2);
  } else if(!strcasecmp(str, "/away")) {
    return away_irc_conn(vc, make_list(vc, 2, hirc, make_raw_string(vc, "away")), 2);
    //return away_irc_conn(vc, cons(vc, hirc, NULL), 1);
  } else if(!strcasecmp(str, "/back")) {
    return back_irc_conn(vc, hirc);
  } else if(!strncasecmp(str, "/topic ", 7) && strlen(str) > 7 && chan) {
    ln = str + 7;
    return topic_irc_conn(vc, make_list(vc, 3, hirc, chan, make_raw_string(vc, ln)), 3);
  } else if(!strcasecmp(str, "/topic") && chan) {
    return topic_irc_conn(vc, make_list(vc, 2, hirc, chan), 2);
  } else if(!strncasecmp(str, "/quit ", 6) && strlen(str) > 6) {    
    ln = str + 6;
    return quit_irc_conn(vc, hirc, make_raw_string(vc, ln));
  } else if(!strcasecmp(str, "/quit")) {
    return quit_irc_conn(vc, hirc, make_raw_string(vc, "client quit"));
  } else if(!strncmp(str, "/", 1)) {
    harangue_display_error_printf("Unknown command: %s", str);
  } else if(harangue_engine->mode == MODE_MSG) { // MSG Mode
    harangue_display_private_me(harangue_engine->msg_mode_nick->buff, str);
    return privmsg_irc_conn(vc, hirc, make_raw_string(vc, harangue_engine->msg_mode_nick->buff), make_raw_string(vc, str));
  } else if(real_chan) { // Everything else, dump to the channel
    harangue_display_public_me(str);
    return privmsg_irc_conn(vc, hirc, chan, make_raw_string(vc, str));
  }
  
  return vc->novalue;
}

VCSI_OBJECT harangue_command_parse_vcsi(VCSI_CONTEXT vc,
					VCSI_OBJECT x) {
  check_arg_type(vc, x, STRING, "harangue-command-parse");

  return harangue_command_parse(vc, STR(x));
}

VCSI_OBJECT harangue_colorize_nick_vcsi(VCSI_CONTEXT vc,
					VCSI_OBJECT x) {
  char buff[1024];
  int fg, bold, c;

  check_arg_type(vc, x, STRING, "harangue-colorize-nick");

  if(harangue_engine->colors == 256) {
    c = harangue_256color_nick_color(STR(x));
  } else {
    harangue_nick_color(STR(x),&fg,&bold);
    c = harangue_mirc_color_map(fg, bold);
  }

  snprintf(buff, 1023, "%d%s0", c, STR(x));

  return make_raw_string(vc, buff);
}

VCSI_OBJECT harangue_window_title_set_vcsi(VCSI_CONTEXT vc,
					   VCSI_OBJECT x) {

  check_arg_type(vc, x, STRING, "window-title-set!");
  harangue_display_set_title(STR(x));
  return vc->true;
}

VCSI_OBJECT harangue_window_icon_set_vcsi(VCSI_CONTEXT vc,
					   VCSI_OBJECT x) {
  check_arg_type(vc, x, STRING, "window-icon-set!");
  harangue_display_set_icon(STR(x));
  return vc->true;
}

char* harangue_print_object_vcsi(VCSI_CONTEXT vc,
				 INFOSTD_DYN_STR result,
				 VCSI_OBJECT item, 
				 int clear) {
  
  VCSI_OBJECT tmp, tmp2;
  unsigned long l;
  int i;

  infostd_dyn_str_clear(vc->res_extra);
  
  if(clear)
    infostd_dyn_str_clear(result);
  
  if(vc->got_error)
    item = vc->errobj;
   
  switch TYPE(item) {
  case 0:
    infostd_dyn_str_strcat(result,"242;()0;");
    break;
  case BOOLEAN:
    if(BOOL(item))
      infostd_dyn_str_strcat(result,"108;#t0;");
    else
      infostd_dyn_str_strcat(result,"108;#f0;");
    break;
  case LNGNUM:
    infostd_dyn_str_printf(vc->res_extra,"156;%ld0;",LNGN(item));
    infostd_dyn_str_strcat(result,vc->res_extra->buff);
    break;
  case FLTNUM:
    if(FLTN(item) == (long)(FLTN(item)))
      infostd_dyn_str_printf(vc->res_extra,"156;%g.0;",FLTN(item));
    else /* %#g */
      infostd_dyn_str_printf(vc->res_extra,"156;%g0;",FLTN(item));
    infostd_dyn_str_strcat(result,vc->res_extra->buff);
    break;
  case RATNUM:
    infostd_dyn_str_printf(vc->res_extra,"156;%d/%d0;",RNUM(item),RDENOM(item));
    infostd_dyn_str_strcat(result,vc->res_extra->buff);
    break;
  case CHAR:
    if(CHAR(item) == ' ')
      infostd_dyn_str_printf(vc->res_extra,"#\\Space");
    else if(CHAR(item) == '\n')
      infostd_dyn_str_printf(vc->res_extra,"#\\Linefeed");
    else
      infostd_dyn_str_printf(vc->res_extra,"210;#\\%c",CHAR(item));
    infostd_dyn_str_strcat(result,vc->res_extra->buff);
    break;
  case STRING:
    infostd_dyn_str_strcat(vc->res_extra,"88;");    
    infostd_dyn_str_addchar(vc->res_extra,'\"');
    for(l=0;l<SLEN(item);l++) {
      if(STR(item)[l] == '\"' || STR(item)[l] == '\\') 
	infostd_dyn_str_addchar(vc->res_extra,'\\');	
      infostd_dyn_str_addchar(vc->res_extra,STR(item)[l]);
    }
    infostd_dyn_str_addchar(vc->res_extra,'\"');
    infostd_dyn_str_strcat(vc->res_extra,"0;");
    infostd_dyn_str_strcat(result,vc->res_extra->buff);
    break;
  case SYMBOL:
    if(item == vc->specialobj) 
      infostd_dyn_str_printf(vc->res_extra,"117;#[%s]0;",SNAME(item));
    else if(SCOPE(item) && 0) /* show different scoped vars */
      infostd_dyn_str_printf(vc->res_extra,"33;&%s0;",SNAME(item));
    else {
      tmp2 = envlookup(vc,item,DYNROOT(vc->root_wind));

      if(tmp2)
	tmp2 = VCELL(tmp2);

      switch(TYPE(tmp2)) {
      case PROC0:
      case PROC1:
      case PROC2:
      case PROC3:
      case PROCENV:
      case PROCOPT:
      case PROCX:
      case SYNTAX:
	infostd_dyn_str_printf(vc->res_extra,"117;%s0;",SNAME(item));
	break;
      default:
	if(tmp2 == vc->specialobj) 
	  infostd_dyn_str_printf(vc->res_extra,"117;%s0;",SNAME(item));
	else
	  infostd_dyn_str_printf(vc->res_extra,"33;%s0;",SNAME(item));
	break;
      }   
    }   
    infostd_dyn_str_strcat(result,vc->res_extra->buff);
    break;
  case PROC0:
  case PROC1:
  case PROC2:
  case PROC3:
  case PROCENV:
  case PROCOPT:
  case PROCX:
    infostd_dyn_str_printf(vc->res_extra,"103;#[primitive-procedure %s]0;",
			   item->data.proc.name);
    infostd_dyn_str_strcat(result,vc->res_extra->buff);
    break;
  case CLOSURE:
    infostd_dyn_str_strcat(result,"103;#[closure ");
    if(vc->verbose) {
      harangue_print_object_vcsi(vc,result,CAR(CLSCODE(item)),0);
      infostd_dyn_str_strcat(result," ");
      harangue_print_object_vcsi(vc,result,CAR(CDR(CLSCODE(item))),0);
    } else {
      infostd_dyn_str_printf(vc->res_extra,"%p",item);
      infostd_dyn_str_strcat(result,vc->res_extra->buff);
    }
    infostd_dyn_str_strcat(result,"]0;");
    break;
  case MACRO:
    infostd_dyn_str_strcat(result,"103;#[macro ");
    if(vc->verbose) {
      harangue_print_object_vcsi(vc,result,CAR(CLSCODE(item)),0);
      infostd_dyn_str_strcat(result," ");
      harangue_print_object_vcsi(vc,result,CAR(CDR(CLSCODE(item))),0);
    } else {
      infostd_dyn_str_printf(vc->res_extra,"%p",item);
      infostd_dyn_str_strcat(result,vc->res_extra->buff);
    }
    infostd_dyn_str_strcat(result,"]0;");
    break;
  case SYNTAX:
    infostd_dyn_str_strcat(result,"103;#[syntax ");
    if(vc->verbose) {
      harangue_print_object_vcsi(vc,result,LITERALS(item),0);
      infostd_dyn_str_strcat(result," ");
      harangue_print_object_vcsi(vc,result,PATTERNS(item),0);
    } else {
      infostd_dyn_str_printf(vc->res_extra,"%p",item);
      infostd_dyn_str_strcat(result,vc->res_extra->buff);
    }
    infostd_dyn_str_strcat(result,"]0;");
    break;
  case PROMISED:
  case PROMISEF:
    infostd_dyn_str_printf(vc->res_extra,"103;#[promise %p]0;",item);
    infostd_dyn_str_strcat(result,vc->res_extra->buff);
    break;
  case PORTI:
    infostd_dyn_str_printf(vc->res_extra,"103;#[input-port for %s]0;",PORTP(item));
    infostd_dyn_str_strcat(result,vc->res_extra->buff);
    break;
  case PORTO:
    infostd_dyn_str_printf(vc->res_extra,"103;#[output-port for %s]0;",
			   PORTP(item));
    infostd_dyn_str_strcat(result,vc->res_extra->buff);
    break;
  case PORTB:
    infostd_dyn_str_printf(vc->res_extra,"103;#[i/o-port for %s]0;",PORTP(item));
    infostd_dyn_str_strcat(result,vc->res_extra->buff);
    break;
  case VECTOR:
    infostd_dyn_str_strcat(result,"28;#(0;");
    for(i=0;i<VSIZE(item);i++) {
      if(i != 0 && i != VSIZE(item))
	infostd_dyn_str_strcat(result," ");
      harangue_print_object_vcsi(vc,result, VELEM(item)[i],0);
    }
    infostd_dyn_str_strcat(result,"28;)0;");
    break;
  case CONTINUATION:
    infostd_dyn_str_printf(vc->res_extra,"103;#[continuation %p]0;",item);
    infostd_dyn_str_strcat(result,vc->res_extra->buff);
    break;
  case VALUES:
    infostd_dyn_str_printf(vc->res_extra,"103;#[values %p ",item);
    infostd_dyn_str_strcat(result,vc->res_extra->buff);
    harangue_print_object_vcsi(vc,result,VVALS(item),0);
    infostd_dyn_str_strcat(result,"]0;");
    break;
#ifdef WITH_THREADS
  case THREAD:
    infostd_dyn_str_printf(vc->res_extra,"103;#[thread %p (%s)]0;",item,
			   (THREADPTR(item)->status == VCSI_THREAD_NOT_STARTED) ?
			   "not started" : (THREADPTR(item)->status == VCSI_THREAD_RUNNING)
			   ? "running" : "done");
    infostd_dyn_str_strcat(result,vc->res_extra->buff);
    break;
  case MUTEX:
    infostd_dyn_str_printf(vc->res_extra,"103;#[mutex %p]0;",item);
    infostd_dyn_str_strcat(result,vc->res_extra->buff);
    break;
#endif
  case REGEXPN:
    infostd_dyn_str_printf(vc->res_extra,"103;#[regexp %p]0;",item); 
    infostd_dyn_str_strcat(result,vc->res_extra->buff);
    break;
  case CONS:
    infostd_dyn_str_strcat(result,"242;(0;");
    for(tmp=item;TYPEP(tmp,CONS);tmp=CDR(tmp)) {	    
      harangue_print_object_vcsi(vc,result, CAR(tmp),0);
      if(CDR(tmp) != NULL)
	infostd_dyn_str_strcat(result," ");
    }
    if(TYPEN(tmp,0)) {
      infostd_dyn_str_strcat(result,"242;.0; ");
      harangue_print_object_vcsi(vc,result, tmp,0);
    }
    infostd_dyn_str_strcat(result,"242;)0;");
    break;
  case HASH:
    infostd_dyn_str_printf(vc->res_extra,"103;#[hash %p]0;",item);
    infostd_dyn_str_strcat(result,vc->res_extra->buff);
    break;
  case USERDEF:
    if(UDTYPE(item) == NULL) {
      infostd_dyn_str_printf(vc->res_extra,"103;#[user-defined %p]0;",item); 
      infostd_dyn_str_strcat(result,vc->res_extra->buff);
    } else {
      if(UDTYPE(item)->ud_print)
	UDTYPE(item)->ud_print(vc,result,item);
      else {
	infostd_dyn_str_printf(vc->res_extra,"103;#[user-defined %s]0;",
			       UDTYPE(item)->name); 
	infostd_dyn_str_strcat(result,vc->res_extra->buff);
      }
    }
    break;
  default:
    infostd_dyn_str_printf(vc->res_extra,"103;#[unknown-type %p]0;",item);
    infostd_dyn_str_strcat(result,vc->res_extra->buff);
    break;
  }
  return result->buff;
}

void harangue_rep(VCSI_CONTEXT vc, char* input) {
  VCSI_OBJECT pos; /* stack pointer */
      
  vc->got_error = 0;
      
  if(!sigsetjmp(DYNERR(vc->root_wind),1)) {
    /* Set our stack pinter */
    DYNSTCK(vc->root_wind) = &pos;

    infostd_dyn_str_clear(vc->input);

    /* Parse the input */
    pos = parse_text(vc,input);

    harangue_print_object_vcsi(vc,vc->input,pos,1);
    harangue_display_rep_in(vc->input->buff);

    pos = eval(vc,pos,DYNROOT(vc->root_wind));

    if(!EQ(pos,vc->novalue)) {
      harangue_print_object_vcsi(vc,vc->result,pos,1);
      // Display Output
      harangue_display_rep_out(vc, vc->result->buff);
    }
  } else {
    if(!vc->input->length) {
      harangue_display_rep_in(input);
    }
    harangue_print_object_vcsi(vc,vc->result,vc->errobj,1);
    harangue_display_rep_out(vc, vc->result->buff);
  }
}

void harangue_vcsi_init(VCSI_CONTEXT vc) {
  set_int_proc(vc, "harangue-display", PROCOPT, harangue_port_display);
  set_int_proc(vc, "harangue-display-large", PROC1, harangue_display_doubled_vcsi);
  set_int_proc(vc, "harangue-display-public", PROC3, harangue_display_public_vcsi);
  set_int_proc(vc, "harangue-display-private", PROC3, harangue_display_private_vcsi);
  set_int_proc(vc, "harangue-display-action", PROC3, harangue_display_action_vcsi);
  set_int_proc(vc, "harangue-display-notice", PROC3, harangue_display_notice_vcsi);
  set_int_proc(vc, "harangue-display-nick-change", PROC2, harangue_display_nick_change_vcsi);
  set_int_proc(vc, "harangue-display-system", PROC1, harangue_display_system_vcsi);
  set_int_proc(vc, "harangue-display-generic", PROC1, harangue_display_generic_vcsi);
  set_int_proc(vc, "harangue-display-raw", PROC1, harangue_display_raw_vcsi);
  set_int_proc(vc, "harangue-display-channel-message", PROC3, harangue_display_chan_msg_vcsi);
  set_int_proc(vc, "harangue-display-ctcp", PROC3, harangue_display_ctcp_vcsi);
  set_int_proc(vc, "harangue-string-time", PROC1, harangue_string_time_vcsi);
  set_int_proc(vc, "harangue-command-parse", PROC1, harangue_command_parse_vcsi);
  set_int_proc(vc, "harangue-colorize-nick", PROC1, harangue_colorize_nick_vcsi);
  set_int_proc(vc, "harangue-display-channel-names", PROC2, harangue_display_chan_names);

  set_int_proc(vc, "display", PROCOPT, harangue_port_display);
  set_int_proc(vc, "display-large", PROC1, harangue_display_doubled_vcsi);
  set_int_proc(vc, "display-public", PROC3, harangue_display_public_vcsi);
  set_int_proc(vc, "display-private", PROC3, harangue_display_private_vcsi);
  set_int_proc(vc, "display-action", PROC3, harangue_display_action_vcsi);
  set_int_proc(vc, "display-notice", PROC3, harangue_display_notice_vcsi);
  set_int_proc(vc, "display-nick-change", PROC2, harangue_display_nick_change_vcsi);
  set_int_proc(vc, "display-system", PROC1, harangue_display_system_vcsi);
  set_int_proc(vc, "display-generic", PROC1, harangue_display_generic_vcsi);
  set_int_proc(vc, "display-raw", PROC1, harangue_display_raw_vcsi);
  set_int_proc(vc, "display-channel-message", PROC3, harangue_display_chan_msg_vcsi);
  set_int_proc(vc, "display-channel-names", PROC2, harangue_display_chan_names);
  set_int_proc(vc, "display-ctcp", PROC3, harangue_display_ctcp_vcsi);
  set_int_proc(vc, "string-time", PROC1, harangue_string_time_vcsi);
  set_int_proc(vc, "command-parse", PROC1, harangue_command_parse_vcsi);
  set_int_proc(vc, "colorize", PROC1, harangue_colorize_nick_vcsi);

  set_int_proc(vc, "window-title-set!", PROC1, harangue_window_title_set_vcsi);
  set_int_proc(vc, "window-icon-set!", PROC1, harangue_window_icon_set_vcsi);
  set_int_proc(vc, "comma-format", PROC1, harangue_comma_format_vcsi);
}

/***************************************************************************/

char* harangue_comma_format(INFOSTD_DYN_STR ds) {
  int i, c;

  for(c = 1, i = ds->length - 1; i > 0; i--, c++) {
    if(c == 3) {
      infostd_dyn_str_addchar_at(ds, ',', i);
      c = 0;
    }
  }
  return ds->buff;
}

VCSI_OBJECT harangue_comma_format_vcsi(VCSI_CONTEXT vc,
				       VCSI_OBJECT x) {

  check_arg_type(vc, x, STRING, "comma-format");

  infostd_dyn_str_clear(vc->tmpstr);
  infostd_dyn_str_strcpy(vc->tmpstr, STR(x));
  
  return make_string(vc, harangue_comma_format(vc->tmpstr));
}
  

/***************************************************************************/

void harangue_handler(VCSI_CONTEXT vc) {
  
  VCSI_OBJECT hirc, ev;
  IRC_CONN ircc;
  struct tm* lt;
  int not_done = 1;
  char t;
  int c, d, lm, m, p;
  int i, ls, s;
  int code;
  IRC_STATUS status;

  VCSI_OBJECT pos; /* stack pointer */

  lm = 0;
  ls = 0;

  hirc = harangue_engine->irc;
  ircc = harangue_engine->ircc;

  while(not_done) {

    // Handle Network event
    while(BOOL((ev = event_ready_irc_conn(vc, hirc)))==1) {
      // Error call back
      if(!sigsetjmp(DYNERR(vc->root_wind), 1)) {
	DYNSTCK(vc->root_wind) = &pos;  
	handle_one_event_irc_conn(vc, hirc);
      }
    }

    // Handle keypress
    while((c = harangue_display_getc()) != -1) {
      	  
      // Handle the Keypress
      if(c == '\n') { // Enter

	harangue_engine->cursor = 0;

	if(!harangue_engine->cmdline->length) continue;

	// Save to history
	if(harangue_engine->history_count) {
	  if(strcmp(harangue_engine->cmdline->buff, infostd_dyn_strs_get(harangue_engine->history, harangue_engine->history_count - 1)) != 0) {
	    infostd_dyn_strs_add(harangue_engine->history, harangue_engine->cmdline->buff);
	    harangue_engine->history_count++;
	  }
	} else {
	  infostd_dyn_strs_add(harangue_engine->history, harangue_engine->cmdline->buff);
	  harangue_engine->history_count++;
	}
	harangue_engine->history_position = harangue_engine->history_count;
	
	if(harangue_engine->cmdline->buff[0] == '(' || 
	   (harangue_engine->mode == MODE_SCHEME && harangue_engine->cmdline->buff[0] != '/')) {
	  // Eval
	  harangue_rep(vc, harangue_engine->cmdline->buff);
	} else {
	  // Send and Display
	  if(!sigsetjmp(DYNERR(vc->root_wind), 1)) {
	    DYNSTCK(vc->root_wind) = &pos;  
	    harangue_command_parse(vc, harangue_engine->cmdline->buff);
	  }	  
	}
	HARANGUE_CMD_STATE_SET(HARANGUE_CMD_STATE_ENTERED);
	infostd_dyn_str_clear(harangue_engine->cmdline);
      } else if(c == KEY_BACKSPACE || c == 127 || c == 263) { // Backspace
	if(harangue_engine->cmdline->length) {
	  //harangue_display_generic_printf("Attempting delete of %d long cmdline at %d [%d]", harangue_engine->cmdline->length, harangue_engine->cursor, harangue_engine->cursor - 1);
	  if(harangue_engine->cmdline->length == harangue_engine->cursor) {
	    HARANGUE_CMD_STATE_SET(HARANGUE_CMD_STATE_DELETION_END);
	  } else {
	    HARANGUE_CMD_STATE_SET(HARANGUE_CMD_STATE_DELETION_INLINE);
	  }
	  infostd_dyn_str_delchar_at(harangue_engine->cmdline, harangue_engine->cursor - 1);
	  if(harangue_engine->cursor > 0)
	    harangue_engine->cursor--; 
	} else {
	  harangue_config_bell();
	}
      } else if(c == KEY_DC) { // Delete
	if(harangue_engine->cmdline->length) {
	  infostd_dyn_str_delchar_at(harangue_engine->cmdline, harangue_engine->cursor);
	  if(harangue_engine->cursor >= harangue_engine->cmdline->length)
	    harangue_engine->cursor--;
	  HARANGUE_CMD_STATE_SET(HARANGUE_CMD_STATE_DELETION_END); // Unclear
	}
      } else if(c == '') { // CTRL-A - start of line
	harangue_engine->cursor = 0;
	HARANGUE_CMD_STATE_SET(HARANGUE_CMD_STATE_CURSOR_MOVE);
      } else if(c == '') { // CTRL-E - end of line
	harangue_engine->cursor = harangue_engine->cmdline->length;
	HARANGUE_CMD_STATE_SET(HARANGUE_CMD_STATE_CURSOR_MOVE);
      } else if(c == '') { // CTRL-K - kill to end of line; To Do: kill buffer
	if(harangue_engine->cmdline->length) {
	  infostd_dyn_str_kill_from(harangue_engine->cmdline, harangue_engine->cursor);
	  HARANGUE_CMD_STATE_SET(HARANGUE_CMD_STATE_DELETION_MAJOR);
	}
      } else if(c == '') { // CTRL-C - kill the entire line
	harangue_engine->cursor = 0;
	infostd_dyn_str_clear(harangue_engine->cmdline);
	HARANGUE_CMD_STATE_SET(HARANGUE_CMD_STATE_DELETION_MAJOR); // Unclear - reset?
      } else if(c == '') { // CTRL-T - transpose
	if(harangue_engine->cmdline->length > 1 && harangue_engine->cursor > 0) {
	  p = harangue_engine->cursor;
	  if(p == harangue_engine->cmdline->length) 
	    p--;
	  t = harangue_engine->cmdline->buff[p];
	  harangue_engine->cmdline->buff[p] = harangue_engine->cmdline->buff[p - 1];
	  harangue_engine->cmdline->buff[p - 1] = t;
	  HARANGUE_CMD_STATE_SET(HARANGUE_CMD_STATE_DELETION_MAJOR);
	}
      } else if(c == '\t' || c == 9) { // TAB complete
	if(harangue_engine->mode != MODE_MSG && !harangue_engine->cmdline->length) { // Basic /msg completes
	  if(hirc && ircc->last_msg->length) {
	    infostd_dyn_str_printf(harangue_engine->cmdline, "/msg %s ", ircc->last_msg->buff);
	    harangue_engine->cursor = strlen(harangue_engine->cmdline->buff);
	    HARANGUE_CMD_STATE_SET(HARANGUE_CMD_STATE_INSERT_MAJOR);
	  } else {
	    infostd_dyn_str_printf(harangue_engine->cmdline, "/msg ");
	    harangue_engine->cursor = strlen(harangue_engine->cmdline->buff);
	    HARANGUE_CMD_STATE_SET(HARANGUE_CMD_STATE_INSERT_MAJOR);
	  }
	}	  
      } else if(c == '') { // META
	d = harangue_display_getc(); // Get the corresponding key
	if(d == 'x' || d == 'X') { // META-X Scheme Mode
	  harangue_engine->mode = (harangue_engine->mode == MODE_SCHEME) ? MODE_IRC : MODE_SCHEME;
	  HARANGUE_CMD_STATE_SET(HARANGUE_CMD_STATE_MODE_CHANGE);
	} else if(d == 'm' || d == 'M') { // META-M MSG Mode
	  if(harangue_engine->mode != MODE_MSG) {
	    //infostd_dyn_str_strcpy(harangue_engine->ircc->last_msg,"debo"); // HACK
	    if(!ircc->last_msg->length) {
	      harangue_display_warning("No nick for MSG Mode. Set with /MSGMODE <nick>");
	    } else {
	      infostd_dyn_str_strcpy(harangue_engine->msg_mode_nick, ircc->last_msg->buff);
	      harangue_engine->mode = MODE_MSG;
	      HARANGUE_CMD_STATE_SET(HARANGUE_CMD_STATE_MODE_CHANGE);
	    }
	  } else {
	    harangue_engine->mode = MODE_IRC;
	    HARANGUE_CMD_STATE_SET(HARANGUE_CMD_STATE_MODE_CHANGE);
	  }
	} else if(d == 'f' || d == 'F') { // Alt-F
	  for(i = harangue_engine->cursor; i < harangue_engine->cmdline->length - 1; i++) {
	    if(isspace(harangue_engine->cmdline->buff[i]) && !isspace(harangue_engine->cmdline->buff[i+1])) {
	      harangue_engine->cursor = i+1;
	      HARANGUE_CMD_STATE_SET(HARANGUE_CMD_STATE_CURSOR_MOVE);
	      break;
	    }
	  }
	  if(i >= harangue_engine->cmdline->length - 1) {
	    harangue_engine->cursor = harangue_engine->cmdline->length;
	  }
	} else if(d == 'b' || d == 'B') { // Alt-B
	  for(i = harangue_engine->cursor - 2; i > 0; i--) {
	    if(isspace(harangue_engine->cmdline->buff[i]) && !isspace(harangue_engine->cmdline->buff[i+i])) {
	      harangue_engine->cursor = i + 1;
	      HARANGUE_CMD_STATE_SET(HARANGUE_CMD_STATE_CURSOR_MOVE);
	      break;
	    }
	  }
	  if(i <= 0) {
	    harangue_engine->cursor = 0;
	    HARANGUE_CMD_STATE_SET(HARANGUE_CMD_STATE_CURSOR_MOVE);
	  }	
	} else if(d == '[') {
	  code = harangue_display_getc();
	  if(code == 'A') { // Up arrow
	    if(harangue_engine->history_count) {
	      harangue_engine->history_position = (harangue_engine->history_position - 1 >= 0) ? harangue_engine->history_position - 1 : harangue_engine->history_count - 1;
	      infostd_dyn_str_clear(harangue_engine->cmdline);
	      infostd_dyn_str_strcpy(harangue_engine->cmdline, infostd_dyn_strs_get(harangue_engine->history, harangue_engine->history_position));
	      harangue_engine->cursor = harangue_engine->cmdline->length;
	      HARANGUE_CMD_STATE_SET(HARANGUE_CMD_STATE_HISTORY_CHANGE);
	    }
	  } else if(code == 'B') { // Down arrow
	    if(harangue_engine->history_count) {
	      harangue_engine->history_position = (harangue_engine->history_position + 1 < harangue_engine->history_count) ? harangue_engine->history_position + 1 : 0;
	      infostd_dyn_str_clear(harangue_engine->cmdline);
	      infostd_dyn_str_strcpy(harangue_engine->cmdline, infostd_dyn_strs_get(harangue_engine->history, harangue_engine->history_position));
	      harangue_engine->cursor = harangue_engine->cmdline->length;
	      HARANGUE_CMD_STATE_SET(HARANGUE_CMD_STATE_HISTORY_CHANGE);
	    }
	  } else if(code == 'C') { // Right arrow
	    if(harangue_engine->cursor < harangue_engine->cmdline->length) {
	      harangue_engine->cursor++;
	      HARANGUE_CMD_STATE_SET(HARANGUE_CMD_STATE_CURSOR_MOVE);
	    } else {
	      harangue_config_bell();
	    }
	  } else if(code == 'D') { // Left arrow
	    if(harangue_engine->cursor > 0) {
	      harangue_engine->cursor--;
	      HARANGUE_CMD_STATE_SET(HARANGUE_CMD_STATE_CURSOR_MOVE);
	    } else {
	      harangue_config_bell();
	    }
	  }
	}
      } else if(isprint(c)) { // Generic 'printable' input
	//harangue_display_generic_printf("Attempting insert / append of %d long cmdline at %d [%d]", harangue_engine->cmdline->length, harangue_engine->cursor, harangue_engine->cursor - 1);
	if(harangue_engine->cmdline->length == harangue_engine->cursor) {
	  HARANGUE_CMD_STATE_SET(HARANGUE_CMD_STATE_CHARACTER_APPEND);
	} else {
	  HARANGUE_CMD_STATE_SET(HARANGUE_CMD_STATE_CHARACTER_INSERT);
	}
	infostd_dyn_str_addchar_at(harangue_engine->cmdline, c, harangue_engine->cursor);
	harangue_engine->cursor++;
      }
      // Redraw the command line if necessary
      if(harangue_engine->cmd_state != HARANGUE_CMD_STATE_NONE) {
	harangue_cmdline_display(harangue_engine->cmdline->buff,  harangue_engine->cursor);
	harangue_engine->cmd_state = HARANGUE_CMD_STATE_NONE;
      }      
    }
    
    // Iterate through status changes
    while((status = irc_status_pop(ircc)) != STATUS_NONE) {
      switch(status) {
      case STATUS_NONE:
      case STATUS_IDLE:
	// These are never triggered
	break;
      case STATUS_NOT_IDLE:
      case STATUS_AWAY:
      case STATUS_BACK:
	HARANGUE_STATUS_STATE_SET(HARANGUE_STATUS_STATE_AWAY_CHANGE);
	break;
      case STATUS_NICK_CHANGED:
      case STATUS_JOINED:
      case STATUS_PARTED:
	HARANGUE_STATUS_STATE_SET(HARANGUE_STATUS_STATE_NICK_CHANGE);
	break;
      case STATUS_QUIT:
	not_done = 0;
	break;
      }
    }
        
    // Update the clock & idle time
    // Needs to be more complex clock function with timers
    lt = harangue_localtime();
    m = lt->tm_min;
    s = lt->tm_sec;

    // Minute Increment
    if(m != lm) {
      // Send a PING every 5m if connected    
      if(lm % 5 == 0) 
	irc_raw_ping(ircc);

      lm = m;
      HARANGUE_STATUS_STATE_SET(HARANGUE_STATUS_STATE_TIME_CHANGE);
    }

    // Second Increment
    if(s != ls) {
      //harangue_display_system_printf("Second passed: %d", s - ls);

      ls = s;
    }

    // Redraw Status Bar, if necessary
    if(harangue_engine->status_state != HARANGUE_STATUS_STATE_NONE) {
      HARANGUE_STATUS_DISPLAY();
      harangue_engine->status_state = HARANGUE_STATUS_STATE_NONE;
    }

    usleep(50000);
  }

  harangue_display_cleanup();
  harangue_display_reset();
}

/***************************************************************************/
void harangue_banner(void) { // 47 characters, should attempt centering

  int indent;
  int c = 12; // Blue

  indent = harangue_engine->window_columns - 47;
  indent /= 2;
  indent -= harangue_main_indent();

  if(harangue_engine->colors == 256)
    c = COLOR_256_DODGERBLUE;

  harangue_display_generic_printf("");
  harangue_display_generic_printf("%*s%d  /\\  /\\__ _ _ __ __ _ _ __   __ _ _   _  ___   ", indent,  "", c);
  harangue_display_generic_printf("%*s%d / /_/ / _` | '__/ _` | '_ \\ / _` | | | |/ _ \\  ", indent, "", c);
  harangue_display_generic_printf("%*s%d/ __  / (_| | | | (_| | | | | (_| | |_| |  __/  ", indent, "", c);
  harangue_display_generic_printf("%*s%d\\/ /_/ \\__,_|_|  \\__,_|_| |_|\\__, |\\__,_|\\___|  ", indent, "", c);
  harangue_display_generic_printf("%*s%d                             |___/              ", indent, "", c);
  harangue_display_generic("");
}

/***************************************************************************/

int main(int argc, char** argv) {
  VCSI_CONTEXT vc;
  char tmp[1024];

  // Initialize VCSI with defaults
  vc = vcsi_init(0);

  // Initialize the Harangue Engine and all the sub parts
  harangue_engine_vcsi_init(vc);

  // Initialize Harangue specific VCSI functions
  harangue_vcsi_init(vc);

  // Set Title, Display Banner and Version
  harangue_display_set_title("harangue");
  harangue_banner();
  harangue_display_system_printf("Harangue IRC Client v%d.%d.%d", HARANGUE_VERSION);

  // Load library functions
  harangue_read_eval(vc, "(load \"harangue.scm\")");

  // Hacks
  //harangue_display_private("Sasquatch", "*", "I got nothing.");
  //harangue_display_notice("schemebot", "*", "I got nothing.");
  //harangue_display_public("gary", "*", "I got nothing.");
  //harangue_display_public("debo", "*", "I got nothing.");
  //harangue_display_action("fm", "*", "dances");

  // Draw the empty Status and Cmdlines
  HARANGUE_STATUS_DISPLAY();
  harangue_cmdline_display("", 0);

  // Load user functions
  if(argc == 2) {
    harangue_display_system("Initializing User Settings...");
    memset(tmp, 0, 1024);
    snprintf(tmp, 1023, "(load \"%s\")", argv[1]);
    harangue_read_eval(vc, tmp);
  }

  // Connect
  if(0)
    connect_irc_conn(vc, harangue_engine->irc);
  
  ///////////////////////////////////////////////////////////////////

  // Start the main handler
  harangue_handler(vc);

  return 0;
}
