/*********************************************************************************
 *
 * Harangue Engine: encapsulates configuration and irc connections
 *
 ********************************************************************************/

#define HARANGUE_DEFAULT_SPELLER "/usr/local/share/infostd/english.speller"
#define HARANGUE_DEFAULT_STATUS_SIZE  32
#define HARANGUE_DEFAULT_CMDLINE_SIZE 256
#define HARANGUE_DEFAULT_HISTORY_SIZE 1024

typedef enum harangue_engine_mode {
  MODE_IRC, MODE_SCHEME, MODE_MSG
} HARANGUE_ENGINE_MODE;

typedef struct str_harangue_engine {
  /* Terminal Settings */
  int window_rows;
  int window_columns;
  int line_last;
  int line_status1;
  int line_status2;
  int line_cmd;

  /* Status variables */
  INFOSTD_DYN_STR status1;
  INFOSTD_DYN_STR status2;
  INFOSTD_DYN_STR status3;
  INFOSTD_DYN_STR status4;
  INFOSTD_DYN_STR status5;

  /* Options */
  int options;

  /* Status State */
  int status_state;

  ////////////////////////////////////////
  // Command Line

  /* Command State */
  int cmd_state;

  /* The command line */
  INFOSTD_DYN_STR cmdline;
  int cursor;

  /* Input Mode */
  HARANGUE_ENGINE_MODE mode;

  /* Nick stored for MsgMode */
  INFOSTD_DYN_STR msg_mode_nick;

  /* Kill Ring Buffer */
  INFOSTD_DYN_STR killring;

  /* History */
  INFOSTD_DYN_STRS history;
  int history_count;
  int history_position;
  ////////////////////////////////////////

  /* Color Depth: 16 or 256 */
  int colors;

  /* IRC Connection */
  VCSI_OBJECT irc;
  IRC_CONN ircc;

  /* Speller */
  INFOSTD_SPELLER speller;

  /* Preprocessor Scheme Function */
  VCSI_OBJECT preprocessor;

  /* Clock */
  time_t clock;

  /* Timers */
  unsigned long timer_count;

  HARANGUE_TIMER timers;

} *HARANGUE_ENGINE;
#define SIZE_HARANGUE_ENGINE (sizeof(struct str_harangue_engine))

/* The Engine and vcsi object globals */
extern HARANGUE_ENGINE harangue_engine;
extern VCSI_OBJECT harangue_engine_obj;

/* Option and State Macros */
#define HARANGUE_OPTION_SET(flag) (harangue_engine->options |= flag)
#define HARANGUE_OPTION_UNSET(flag) (harangue_engine->options &= ~flag)
#define HARANGUE_OPTION_CHECK(flag) (harangue_engine->options & flag)

#define HARANGUE_OPTION_NONE            0
#define HARANGUE_OPTION_TIMESTAMP      (1 << 0)
#define HARANGUE_OPTION_HOSTMASK       (1 << 1)
#define HARANGUE_OPTION_SPELLCHECK     (1 << 2)
#define HARANGUE_OPTION_BELL           (1 << 3)
#define HARANGUE_OPTION_UNICODE        (1 << 4)
#define HARANGUE_OPTION_UNICODE_TRICKS (1 << 5)
#define HARANGUE_OPTION_PREPROCESS     (1 << 6)

#define HARANGUE_COLORS(x) (harangue_engine->colors == x)

#define HARANGUE_STATUS_STATE_SET(flag) (harangue_engine->status_state |= flag)
#define HARANGUE_STATUS_STATE_UNSET(flag) (harangue_engine->status_state &= ~flag)
#define HARANGUE_STATUS_STATE_CHECK(flag) (harangue_engine->status_state & flag)

#define HARANGUE_STATUS_STATE_NONE                   0 // Up to Date
#define HARANGUE_STATUS_STATE_VARIABLE_CHANGE  (1 << 0) // One of the Five Variables was altered
#define HARANGUE_STATUS_STATE_NICK_CHANGE      (1 << 1) // The Nick was changed
#define HARANGUE_STATUS_STATE_TIME_CHANGE      (1 << 2) // Clock update from time change
#define HARANGUE_STATUS_STATE_AWAY_CHANGE      (1 << 3) // Away status changed
#define HARANGUE_STATUS_STATE_CHANNEL_CHANGE   (1 << 4) // Current Channel changed
#define HARANGUE_STATUS_STATE_COLOR_CHANGE     (1 << 5) // Color scheme changed

#define HARANGUE_CMD_STATE_SET(flag) (harangue_engine->cmd_state |= flag)
#define HARANGUE_CMD_STATE_UNSET(flag) (harangue_engine->cmd_state &= ~flag)
#define HARANGUE_CMD_STATE_CHECK(flag) (harangue_engine->cmd_state & flag)

#define HARANGUE_CMD_STATE_NONE                   0 // Up to Date
#define HARANGUE_CMD_STATE_ENTERED          (1 << 0) // Enter hit on a cmdline
#define HARANGUE_CMD_STATE_DELETION_END     (1 << 1) // Backspace at end
#define HARANGUE_CMD_STATE_DELETION_INLINE  (1 << 2) // Backspace / Delete inlin
#define HARANGUE_CMD_STATE_CURSOR_MOVE      (1 << 3) // Cursor Moved
#define HARANGUE_CMD_STATE_DELETION_MAJOR   (1 << 4) // Kill to the end of line or whole line
#define HARANGUE_CMD_STATE_CHARACTER_APPEND (1 << 5) // Single character insert
#define HARANGUE_CMD_STATE_CHARACTER_INSERT (1 << 6) // Single character insert
#define HARANGUE_CMD_STATE_INSERT_MAJOR     (1 << 7) // Multiple character insert
#define HARANGUE_CMD_STATE_HISTORY_CHANGE   (1 << 8) // History change
#define HARANGUE_CMD_STATE_MODE_CHANGE      (1 << 9) // Switch between Modes

/* VCSI User-Defined Type */
VCSI_USER_DEF harangue_engine_type;
#define HENG(x) ((HARANGUE_ENGINE)(*x).data.userdef.ud_value)

HARANGUE_ENGINE harangue_engine_init(void);
void harangue_engine_free(HARANGUE_ENGINE hen);

void harangue_engine_ud_print(VCSI_CONTEXT vc,
			      INFOSTD_DYN_STR res, 
			      VCSI_OBJECT obj);

void harangue_engine_ud_mark(VCSI_CONTEXT vc,
			     VCSI_OBJECT obj, 
			     int flag);

void harangue_engine_ud_free(VCSI_CONTEXT vc,
			     VCSI_OBJECT obj);

VCSI_OBJECT harangue_engine_ud_eval(VCSI_CONTEXT vc,
				    VCSI_OBJECT args, 
				    int length);

VCSI_OBJECT harangue_engine_make(VCSI_CONTEXT vc);

void harangue_engine_vcsi_init(VCSI_CONTEXT vc);

int harangue_engine_file_exists(char *s);
