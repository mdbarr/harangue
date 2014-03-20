#include "harangue.h"

HARANGUE_ENGINE harangue_engine = NULL;
VCSI_OBJECT harangue_engine_obj = NULL;

HARANGUE_ENGINE harangue_engine_init(void) {
  HARANGUE_ENGINE hen;

  hen = MALLOC(SIZE_HARANGUE_ENGINE);
  memset(hen, 0, SIZE_HARANGUE_ENGINE);

  hen->status1 = infostd_dyn_str_init(HARANGUE_DEFAULT_STATUS_SIZE);
  hen->status2 = infostd_dyn_str_init(HARANGUE_DEFAULT_STATUS_SIZE);
  hen->status3 = infostd_dyn_str_init(HARANGUE_DEFAULT_STATUS_SIZE);
  hen->status4 = infostd_dyn_str_init(HARANGUE_DEFAULT_STATUS_SIZE);
  hen->status5 = infostd_dyn_str_init(HARANGUE_DEFAULT_STATUS_SIZE);

  hen->options = HARANGUE_OPTION_NONE;

  hen->status_state = HARANGUE_STATUS_STATE_NONE;

  //
  hen->cmd_state = HARANGUE_CMD_STATE_NONE;

  hen->cmdline = infostd_dyn_str_init(HARANGUE_DEFAULT_CMDLINE_SIZE);

  hen->mode = MODE_IRC;

  hen->msg_mode_nick = infostd_dyn_str_init(HARANGUE_DEFAULT_STATUS_SIZE);

  hen->killring = infostd_dyn_str_init(HARANGUE_DEFAULT_CMDLINE_SIZE);

  hen->history = infostd_dyn_strs_init(HARANGUE_DEFAULT_CMDLINE_SIZE);
  //

  return hen;
}

void harangue_engine_free(HARANGUE_ENGINE hen) {
  if(hen) {
    if(hen->status1)
      infostd_dyn_str_free(hen->status1);
    if(hen->status2)
      infostd_dyn_str_free(hen->status2);
    if(hen->status3)
      infostd_dyn_str_free(hen->status3);
    if(hen->status4)
      infostd_dyn_str_free(hen->status4);
    if(hen->status5)
      infostd_dyn_str_free(hen->status5);
    if(hen->msg_mode_nick)
      infostd_dyn_str_free(hen->msg_mode_nick);
  }
}

void harangue_engine_ud_print(VCSI_CONTEXT vc,
			      INFOSTD_DYN_STR res, 
			      VCSI_OBJECT obj) {

  infostd_dyn_str_printf(vc->res_extra, "103;#[harangue-engine]0;");
  
  infostd_dyn_str_strcat(res, vc->res_extra->buff);
}

void harangue_engine_ud_mark(VCSI_CONTEXT vc,
			     VCSI_OBJECT obj, 
			     int flag) {

  gc_mark(vc, HENG(obj)->irc, flag);
  gc_mark(vc, HENG(obj)->preprocessor, flag);

}

void harangue_engine_ud_free(VCSI_CONTEXT vc,
			     VCSI_OBJECT obj) {
  
  // Shouldn't ever be called, but should still implement
}

VCSI_OBJECT harangue_engine_ud_eval(VCSI_CONTEXT vc,
				    VCSI_OBJECT args, 
				    int length) {

  VCSI_OBJECT method, arg1, arg2;
  char *mstr;
  INFOSTD_DYN_STR st;
  int mlen;
  int snum;
 
  method = arg1 = arg2 = NULL;

  if(length < 1 || length > 4)
    error(vc, "invalid number of arguments to irc-conn", args);

  // No Method return the object
  if(length < 2)
    return harangue_engine_obj;

  // Setup a new list of the arguments apart from the method
  //arg = cons(vc, harangue_engine_obj, cddr(vc, args));
  //alen = get_length(arg);

  // Find the Method and setup a character pointer to it's content
  method = cadr(vc,args);
  mstr = check_arg_string_like(vc, method, "irc-conn");
  mlen = strlen(mstr);
  if(!mlen)
    error(vc, "invalid method to irc-conn", method);

  // Setup the other arguments, only if present
  if(length > 2)
    arg1 = caddr(vc,args);
  if(length > 3)
    arg2 = cadddr(vc,args);

  // Parse and attempt to handle the Method
  if(mlen == 7 && !strncasecmp(mstr, "status", 6)) { // Status Variable
    snum = mstr[6] - '0';
    if(snum == 1) {
      st = harangue_engine->status1;
    } else if(snum == 2) {
      st = harangue_engine->status2;
    } else if(snum == 3) {
      st = harangue_engine->status3;
    } else if(snum == 4) {
      st = harangue_engine->status4;
    } else if(snum == 5) {
      st = harangue_engine->status5;
    } else 
      return error(vc, "invalid status variable to harangue-engine", method);

    // Set the status if present
    if(arg1) {
      HARANGUE_STATUS_STATE_SET(HARANGUE_STATUS_STATE_VARIABLE_CHANGE);
      if(TYPEP(arg1,STRING)) {
	  infostd_dyn_str_strcpy(st, STR(arg1));
      } else {
	print_obj(vc, st, arg1, 1);
      }
    }
    // Return the string status
    return make_string(vc, st->buff);
  } else if(!strcasecmp(mstr, "version")) { // Version
    return make_string(vc, harangue_version);
  } else if(!strcasecmp(mstr, "timestamp")) { // Timestamp option
    if(arg1) {
      if(!eq(vc,arg1,vc->false)) {
	HARANGUE_OPTION_SET(HARANGUE_OPTION_TIMESTAMP);
      } else {
	HARANGUE_OPTION_UNSET(HARANGUE_OPTION_TIMESTAMP);
      }
    }
    return (HARANGUE_OPTION_CHECK(HARANGUE_OPTION_TIMESTAMP)) ? vc->true : vc->false;
  } else if(!strcasecmp(mstr, "hostmask")) { // Hostmask option
    if(arg1) {
      if(!eq(vc,arg1,vc->false)) {
	HARANGUE_OPTION_SET(HARANGUE_OPTION_HOSTMASK);
      } else {
	HARANGUE_OPTION_UNSET(HARANGUE_OPTION_HOSTMASK);
      }
    }
    return (HARANGUE_OPTION_CHECK(HARANGUE_OPTION_HOSTMASK)) ? vc->true : vc->false;
  } else if(!strcasecmp(mstr, "bell") || !strcasecmp(mstr, "beep")) { // Bell option
    if(arg1) {
      if(!eq(vc,arg1,vc->false)) {
	HARANGUE_OPTION_SET(HARANGUE_OPTION_BELL);
      } else {
	HARANGUE_OPTION_UNSET(HARANGUE_OPTION_BELL);
      }
    }
    return (HARANGUE_OPTION_CHECK(HARANGUE_OPTION_BELL)) ? vc->true : vc->false;
  } else if(!strcasecmp(mstr, "spellcheck")) { // Spellcheck option
    if(arg1) {
      if(!eq(vc,arg1,vc->false) && harangue_engine->speller) {
	HARANGUE_OPTION_SET(HARANGUE_OPTION_SPELLCHECK);
      } else {
	HARANGUE_OPTION_UNSET(HARANGUE_OPTION_SPELLCHECK);
      }
    }
    return (HARANGUE_OPTION_CHECK(HARANGUE_OPTION_SPELLCHECK)) ? vc->true : vc->false;
  } else if(!strcasecmp(mstr, "unicode")) { // Unicode option
    if(arg1) {
      if(!eq(vc,arg1,vc->false)) {
	HARANGUE_OPTION_SET(HARANGUE_OPTION_UNICODE);
      } else {
	HARANGUE_OPTION_UNSET(HARANGUE_OPTION_UNICODE);
      }
    }
    return (HARANGUE_OPTION_CHECK(HARANGUE_OPTION_UNICODE)) ? vc->true : vc->false;
  } else if(!strcasecmp(mstr, "tricks")) { // Unicode Tricks option
    if(arg1) {
      if(!eq(vc,arg1,vc->false)) {
	HARANGUE_OPTION_SET(HARANGUE_OPTION_UNICODE_TRICKS);
      } else {
	HARANGUE_OPTION_UNSET(HARANGUE_OPTION_UNICODE_TRICKS);
      }
    }
    return (HARANGUE_OPTION_CHECK(HARANGUE_OPTION_UNICODE_TRICKS)) ? vc->true : vc->false;
  } else if(!strcasecmp(mstr, "colors")) { // Color depth
    if(arg1) {
      if(TYPEP(arg1, LNGNUM)) {
	if(LNGN(arg1) == 16) {
	  harangue_engine->colors = 16;
	  HARANGUE_STATUS_STATE_SET(HARANGUE_STATUS_STATE_COLOR_CHANGE);
	} else if(LNGN(arg1) == 256) {
	  harangue_engine->colors = 256;
	  HARANGUE_STATUS_STATE_SET(HARANGUE_STATUS_STATE_COLOR_CHANGE);
	} else {
	  return error(vc, "invalid color depth to harangue-engine", arg1);
	}
      } else {
	return error(vc, "invalid color depth to harangue-engine", arg1);
      }
    }
    return make_long(vc, harangue_engine->colors);
  } else { // Pass everything else off to the IRC object
    CAR(args) = harangue_engine->irc;
    return eval_irc_conn(vc, args, length);
  }
}


VCSI_OBJECT harangue_engine_make(VCSI_CONTEXT vc) {
  // Create the Engine
  harangue_engine = harangue_engine_init();

  // Setup the Harangue screen environment
  harangue_display_reset();
  harangue_display_setup();

  harangue_engine->window_rows = harangue_window_rows;
  harangue_engine->window_columns = harangue_window_cols;

  harangue_engine->line_last = harangue_window_rows - 3;
  harangue_engine->line_status1 = harangue_window_rows - 2;
  harangue_engine->line_status2 = harangue_window_rows - 1;
  harangue_engine->line_cmd = harangue_window_rows;

  // Sanity check the screen environment
  if(harangue_engine->window_rows < 8 || harangue_engine->window_columns < 40) {
    harangue_display_cleanup();
    harangue_display_reset();
    printf("harangue: terminal too small, I give up!\n");
    exit(0);
  }

  // Set the scrolling range
  harangue_display_scroll_set(1, harangue_engine->line_last);

  // Set the default colors to 16
  harangue_engine->colors = 16;

  // Check for 256 Colors from TERM environment variable
  if(getenv("TERM") != NULL && strstr(getenv("TERM"), "256color") != NULL)
    harangue_engine->colors = 256;

  if(getenv("LANG") != NULL && strstr(getenv("LANG"), "UTF-8") != NULL) {
    HARANGUE_OPTION_SET(HARANGUE_OPTION_UNICODE);
    HARANGUE_OPTION_SET(HARANGUE_OPTION_UNICODE_TRICKS);
  }

  // Create and register the IRC connections
  irc_conn_type = make_user_def_type(vc, "harangue-irc", print_irc_conn, 
				     mark_irc_conn, free_irc_conn);
  harangue_engine->irc = init_irc_conn(vc);
  harangue_engine->ircc = IRCC(harangue_engine->irc);

  enable_status_queue_irc_conn(vc,harangue_engine->irc);

  // Create the User-Defined Object
  harangue_engine_obj = make_user_def(vc, (void*) harangue_engine, harangue_engine_type);

  // Set the custom Evaluator
  user_def_set_eval(vc, harangue_engine_obj, PROCOPT, harangue_engine_ud_eval);

  // Set default options
  HARANGUE_OPTION_SET(HARANGUE_OPTION_TIMESTAMP);
  HARANGUE_OPTION_SET(HARANGUE_OPTION_BELL);

  // Register it
  setvar(vc, make_internal(vc, "harangue"), harangue_engine_obj, NULL);

  return harangue_engine_obj;
}

///////////////////////////////////////////////////////////////////////////

VCSI_OBJECT harangue_engine_speller_load(VCSI_CONTEXT vc,
					 VCSI_OBJECT x) {

  FILE *fp;
  char *sp;

  if(x == NULL) {
    sp = HARANGUE_DEFAULT_SPELLER;
  } else {
    check_arg_type(vc, x, STRING, "harangue-speller-load");
    sp = STR(x);
  }

  // Check and free old speller
  if(harangue_engine->speller) {
    infostd_speller_free(harangue_engine->speller);
    harangue_engine->speller = NULL;
    HARANGUE_OPTION_UNSET(HARANGUE_OPTION_SPELLCHECK);
  }

  // Load the Speller
  fp = fopen(sp, "r");
  if(fp) {
    harangue_display_system("Initializing Spell Checking...");
    harangue_engine->speller = infostd_speller_read(fp);
    fclose(fp);
    if(harangue_engine->speller) {
      infostd_dyn_str_clear(vc->tmpstr);
      infostd_dyn_str_printf(vc->tmpstr, "%ld", harangue_engine->speller->dictionary->num_elem);
      harangue_display_system_printf("Spell Checker loaded: %s words.", harangue_comma_format(vc->tmpstr));
      HARANGUE_OPTION_SET(HARANGUE_OPTION_SPELLCHECK);
      return vc->true;
    } else {
      harangue_display_warning("Spell Checker failed to load.");
      return vc->false;
    }
  } else {
    harangue_display_warning_printf("Spell Checker couldn't open file \'%s\'.", sp);
    return vc->false;
  }
  return vc->false;
}

///////////////////////////////////////////////////////////////////////////

void harangue_engine_vcsi_init(VCSI_CONTEXT vc) {
  harangue_engine_type = make_user_def_type(vc, "harangue-engine", 
					    harangue_engine_ud_print,
					    harangue_engine_ud_mark,
					    harangue_engine_ud_free);

  set_int_proc(vc, "harangue-speller-load", PROC1, harangue_engine_speller_load);

  harangue_engine_make(vc);
}
///////////////////////////////////////////////////////////////////////////

int harangue_engine_file_exists(char *s) {
  struct stat st;
  if ((!s)||(!*s)) return 0;
  if (stat(s,&st)==-1) return 0;
  return 1;
}
