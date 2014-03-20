typedef struct struct_harangue_timer {

  // Numeric ID
  unsigned long id;
  
  // Timer active and counting down
  short active;

  // How many times to repeat, 0 for infinite
  int repetitions;
  // Repetitions remaining
  int repetitions_remaining;

  // Interval between executions
  int interval;
  // Current Interval countdown
  int interval_remaining;

  // String to execute
  INFOSTD_DYN_STR command;

  // VCSI Object to evaluate
  VCSI_OBJECT code;

  struct struct_harangue_timer* next;  
} *HARANGUE_TIMER;
#define SIZE_HARANGUE_TIMER (sizeof(struct struct_harangue_timer))

/*

Uses:
  /timer 0 10 hello
  /timer 5 1 /msg gary hello
  /timer [repetitions] [interval] command
  /timer stop 1
  /timer start 1
  
*/

HARANGUE_TIMER harangue_timer_init(void);
void harangue_timer_free(HARANGUE_TIMER ht);
