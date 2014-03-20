#include "harangue.h"

// Displays a String with a unicode combining character over every alphanumeric
int harangue_display_unicode_combined(char* s, unsigned long ch, int color) {
  int i, l;
  
  l = strlen(s);

  for(i = 0; i < l; i++) {
    printf("%c", s[i]);
    if(isalnum(s[i])) {
      if(color != -1) harangue_display_256color_foreground(color); // Assumes 256 Color
      harangue_display_unicode_character(ch);
      if(color != -1) harangue_display_attribute(ATTR_NONE);
    }
  }
  return l;
}

int harangue_display_unicode_underline(char* s, int color) {
  return harangue_display_unicode_combined(s, UNICODE_COMBINING_LOWLINE, color);
}

int harangue_display_unicode_strikethrough(char* s, int color) {
  return harangue_display_unicode_combined(s, UNICODE_COMBINING_OVERLAY, color);
}

void harangue_display_emit_byte(unsigned long c) {
  printf("%c", (unsigned int) c);
}

void harangue_display_unicode_character(unsigned long c) {
    if (c <= 0x7F) {
      harangue_display_emit_byte(c);
    } else if (c <= 0x7FF) {
      harangue_display_emit_byte(0xC0 | (c>>6));
      harangue_display_emit_byte(0x80 | (c & 0x3F));
    } else if (c <= 0xFFFF) {
      harangue_display_emit_byte(0xE0 | (c>>12));
      harangue_display_emit_byte(0x80 | (c>>6 & 0x3F));
      harangue_display_emit_byte(0x80 | (c & 0x3F));
    } else if (c <= 0x1FFFFF) {
      harangue_display_emit_byte(0xF0 | (c>>18));
      harangue_display_emit_byte(0x80 | (c>>12 & 0x3F));
      harangue_display_emit_byte(0x80 | (c>>6 & 0x3F));
      harangue_display_emit_byte(0x80 | (c & 0x3F));
    } else if (c <= 0x3FFFFFF) {
      harangue_display_emit_byte(0xF8 | (c>>24));
      harangue_display_emit_byte(0x80 | (c>>18 & 0x3F));
      harangue_display_emit_byte(0x80 | (c>>12 & 0x3F));
      harangue_display_emit_byte(0x80 | (c>>6 & 0x3F));
      harangue_display_emit_byte(0x80 | (c & 0x3F));
    } else if (c <= 0x7FFFFFFF) {
      harangue_display_emit_byte(0xFC | (c>>30));
      harangue_display_emit_byte(0x80 | (c>>24 & 0x3F));
      harangue_display_emit_byte(0x80 | (c>>18 & 0x3F));
      harangue_display_emit_byte(0x80 | (c>>12 & 0x3F));
      harangue_display_emit_byte(0x80 | (c>>6 & 0x3F));
      harangue_display_emit_byte(0x80 | (c & 0x3F));
    } else {
      //if a character does meet any of the conditions
      //it outprints a UTF-8 REPLACEMENT CHARACTER
      harangue_display_emit_byte(0xEFBFBD);
    }
}

/*
void main(void) {

  harangue_display_unicode_character(0x1f617); printf("  ");
  harangue_display_unicode_character(0x1f604); printf("  ");
  harangue_display_unicode_character(0x1f620); printf("  ");
  harangue_display_unicode_character(0x1f3ae); printf("  ");
  harangue_display_unicode_character(0x1f4e5); printf("  ");
  harangue_display_unicode_character(0x1f36c); printf("  ");
  printf("\n");

  harangue_display_unicode_character(UNICODE_LAMBDA);
  printf(" (+ 1 1)\n");
  harangue_display_unicode_underline("this is a test, sorry\n", COLOR_256_RED);
  harangue_display_unicode_underline("one more test because everyone likes blue\n", COLOR_256_DODGERBLUE);
  harangue_display_unicode_strikethrough("this is another test\n", COLOR_256_RED);
  harangue_display_unicode_strikethrough("this is another test, because BLUE\n", COLOR_256_DODGERBLUE);
}
*/
