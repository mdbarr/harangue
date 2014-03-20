#include "harangue.h"

HARANGUE_TIMER harangue_timer_init(void) {
  HARANGUE_TIMER ht;

  ht = MALLOC(SIZE_HARANGUE_TIMER);
  memset(ht, 0, SIZE_HARANGUE_TIMER);

  return ht;
}

void harangue_timer_free(HARANGUE_TIMER ht) {
  if(ht) {
    if(ht->command)
      infostd_dyn_str_free(ht->command);
    FREE(ht);
  }
}

HARANGUE_TIMER harangue_timer_make(unsigned long id,
				   int repetitions,
				   int interval,
				   char* command,
				   VCSI_OBJECT code) {

  HARANGUE_TIMER ht;

  ht = harangue_timer_init();

  ht->id = id;
  ht->active = 1;

  ht->repetitions = repetitions;
  ht->repetitions_remaining = repetitions;

  ht->interval = interval;
  ht->interval_remaining = interval;

  return ht;
}
