int harangue_display_unicode_combined(char* s, unsigned long ch, int color);
int harangue_display_unicode_underline(char* s, int color);
int harangue_display_unicode_strikethrough(char* s, int color);
void harangue_display_emit_byte(unsigned long c);
void harangue_display_unicode_character(unsigned long c);

#define UNICODE_LAMBDA 0x03bb

#define UNICODE_SUN 0x2600
#define UNICODE_CLOUD 0x2601
#define UNICODE_UMBRELLA 0x2602
#define UNICODE_SNOWFLAKE 0x2744

#define UNICODE_DEGREE_SIGN 0x00b0

#define UNICODE_COFFEE 0x2615

#define UNICODE_CHECK 0x2714
#define UNICODE_CROSS 0x2716

#define UNICODE_CHECK_THIN 0x2713
#define UNICODE_CROSS_THIN 0x2717

#define UNICODE_FLAG 0x2691

#define UNICODE_ARROW_RIGHT 0x279c // 0x2794

#define UNICODE_SUN_CLOUD_COLOR 0x26c5
#define UNICODE_UMBRELL_RAIN_COLOR 0x2614

#define UNICODE_COMBINING_LOWLINE 0x0332
#define UNICODE_COMBINING_OVERLAY 0x0336

#define UNICODE_DOUBLE_QUOTE_LEFT 0x201c
#define UNICODE_DOUBLE_QUOTE_RIGHT 0x201d

#define UNICODE_SINGLE_QUOTE_LEFT 0x2018
#define UNICODE_SINGLE_QUOTE_RIGHT 0x2019

#define UNICODE_DOUBLE_ANGLE_LEFT 0x00ab
#define UNICODE_DOUBLE_ANGLE_RIGHT 0x00bb

