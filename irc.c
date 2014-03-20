#include <vcsi/vcsi.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pwd.h>
#include <errno.h>

#include "irc.h"

extern int errno;

///////////////////////////////////////////////////////////////////////////
// Event Functions

IRC_EVENT irc_event_init(void) {
  IRC_EVENT irce = NULL;
  
  irce = MALLOC(SIZE_IRC_EVENT);
  memset(irce,0,SIZE_IRC_EVENT);
  
  irce->channel = infostd_dyn_str_init(IRC_DYNSTR_DEFAULT);
  irce->target = infostd_dyn_str_init(IRC_DYNSTR_DEFAULT);
  irce->nick = infostd_dyn_str_init(IRC_DYNSTR_DEFAULT);
  irce->host = infostd_dyn_str_init(IRC_DYNSTR_DEFAULT);
  irce->text = infostd_dyn_str_init(IRC_DYNSTR_TEXT_DEFAULT);
  
 return irce;
}

void irc_event_free(IRC_EVENT irce) {
  if(irce) {
    if(irce->channel)
      infostd_dyn_str_free(irce->channel);
    if(irce->target)
      infostd_dyn_str_free(irce->target);
    if(irce->nick)
      infostd_dyn_str_free(irce->nick);
    if(irce->host)
      infostd_dyn_str_free(irce->host);
    if(irce->text)
      infostd_dyn_str_free(irce->text);
    FREE(irce);
  }
}

IRC_EVENT parse_event(char* etext) {
  IRC_EVENT irce = NULL;
  char *nick, *host, *type, *target, *target2, *text;
  int len, dtype;
  int i;

  nick = host = type = target = target2 = text = NULL;
  
  irce = irc_event_init();

  if(!etext) {
    irce->type = DISCONNECT;
    return irce;
  }

  if((len = strlen(etext))) { /* strip extra garbage */
    if(etext[len-1] == '\r' || etext[len-1] == '\n')
      etext[len-1] = '\0';
  }
  
  if(etext[0] == ':') {
    nick = etext; /* get the nick */
    nick++;
    
    type = strchr(nick,' '); /* get the type */
    if(type) {
      *type='\0';
      type++;
      
      target = strchr(type,' '); /* get the target */
      if(target) {
	*target='\0';
	target++;
	
	if(*target==':') {
	  text=target;
	  text++;
	  target=NULL;

	} else {
	  text = strchr(target,' ');
	  
	  if(text) {
	    *text='\0';
	    text++;
	    if(*text==':')
	      text++;
	    else {
	      target2=text;
	      if(*target2 == '=' || *target2 == '@')
		target2+=2;

	      text=strchr(target2,' ');
	      if(text) {
		*text='\0';
		text++;
		if(*text==':')
		  text++;
	      }
	      
	    }
	  }
	}
      }
    }
    
    if(nick && (host=strchr(nick,'!'))) {    
      *host='\0';
      host++;
    } else {
      host=nick;
      nick=NULL;
    }
    
    /* Did the text come though as the second target? */
    if(!text) {
      if(target2) {
	text=target2;
	target2=NULL;
      }
    }

    /* strip extra garbage */
    if(text && (len = strlen(text))) {
      if(text[len-1] == '\r' || text[len-1] == '\n')
	text[len-1] = '\0';
    }

    /* DEBUG */
    /*
      fprintf(stderr,"GOT: %s %s %s %s %s %s\n",
      nick,host,type,target,target2,text);
    */
    if(type)
      dtype = atol(type);
    else
      dtype = 0;

    if(!strcmp(type,"001")) { /* connect */
      irce->type = CONNECT;
      
      if(target)
	infostd_dyn_str_strcpy(irce->target,target);
      if(nick)
	infostd_dyn_str_strcpy(irce->nick,nick);	
      if(host)
	infostd_dyn_str_strcpy(irce->host,host);	
      if(text)
	infostd_dyn_str_strcpy(irce->text,text);	

    } else if(!strcmp(type,"PRIVMSG")) { /* message */
      if(*target=='#') { /* public */
	irce->type = PUBLIC;
	infostd_dyn_str_strcpy(irce->channel,target);	
      } else  /* private */
	irce->type = MSG;
      
      if(target)
	infostd_dyn_str_strcpy(irce->target,target);	
      if(nick)
	infostd_dyn_str_strcpy(irce->nick,nick);
      if(host)
	infostd_dyn_str_strcpy(irce->host,host);	

      if(*text==1) { /* CTCP something */
	text++;
	text[strlen(text)-1]='\0';

	if(!strncmp(text,"ACTION",6)) {
	  irce->type = ACTION;
	  text+=7;
	} else if(!strncmp(text,"VERSION",7)) {
	  irce->type = CTCP_VERSION;
	  text+=8;
	  if(!strncmp(text,"PING",4))
	    text+=5;
	} else if(!strncmp(text,"PING",4)) {
	  irce->type = CTCP_PING;
	  text+=5;
	} else
	  irce->type = CTCP;
      }
      
      infostd_dyn_str_strcpy(irce->text,text);
    } else if(!strcmp(type,"NOTICE")) { /* someone notices */
      irce->type = NOTICE;

      if(target)
	infostd_dyn_str_strcpy(irce->target,target);
      if(nick)
	infostd_dyn_str_strcpy(irce->nick,nick);	
      if(host)
	infostd_dyn_str_strcpy(irce->host,host);	
      if(text)
	infostd_dyn_str_strcpy(irce->text,text);	
    } else if(!strcmp(type,"JOIN")) { /* someone joins */
      irce->type = JOIN;

      if(nick)
	infostd_dyn_str_strcpy(irce->nick,nick);	
      if(host)
	infostd_dyn_str_strcpy(irce->host,host);	
      if(text)
	infostd_dyn_str_strcpy(irce->channel,text);	

    } else if(!strcmp(type,"PART")) { /* someone parts */
      irce->type = PART;

      /*
      fprintf(stderr,"\n\nGOT: n%s h%s tp%s tg%s tg2%s tx%s\n",
      nick,host,type,target,target2,text);

      sleep(5);
      */
      if(nick)
	infostd_dyn_str_strcpy(irce->nick,nick);	
      if(host)
	infostd_dyn_str_strcpy(irce->host,host);	
      if(target)
	infostd_dyn_str_strcpy(irce->channel,target);	

    } else if(!strcmp(type,"QUIT")) { /* someone quits */
      irce->type = QUIT;

      if(nick)
	infostd_dyn_str_strcpy(irce->nick,nick);	
      if(host)
	infostd_dyn_str_strcpy(irce->host,host);	
      if(text)
	infostd_dyn_str_strcpy(irce->text,text);	

    } else if(!strcmp(type,"NICK")) { /* nick change */
      irce->type = NICK_CHANGE;

      if(nick)
	infostd_dyn_str_strcpy(irce->nick,nick);	
      if(host)
	infostd_dyn_str_strcpy(irce->host,host);	
      if(text)
	infostd_dyn_str_strcpy(irce->text,text);	
    } else if(!strcmp(type,"PONG")) {
      irce->type = PONG;

      if(host)
	infostd_dyn_str_strcpy(irce->host,host);	
      if(target)
	infostd_dyn_str_strcpy(irce->channel,target);	
      if(text)
	infostd_dyn_str_strcpy(irce->text,text);	
    } else if(!strcmp(type,"PING")) {
      irce->type = PING;

      if(host)
	infostd_dyn_str_strcpy(irce->host,host);	
      if(target)
	infostd_dyn_str_strcpy(irce->channel,target);	
      if(text)
	infostd_dyn_str_strcpy(irce->text,text);	
    } else if(!strcmp(type,"433")) { /* nick already taken */
      irce->type = NICK_TAKEN;

      if(host)
	infostd_dyn_str_strcpy(irce->host,host);	
      if(target2)
	infostd_dyn_str_strcpy(irce->target,target2);	
      if(text)
	infostd_dyn_str_strcpy(irce->text,text);	

    } else if(!strcmp(type,"KICK")) { /* when someone is kicked */
      irce->type = KICK;

      if(nick)
	infostd_dyn_str_strcpy(irce->nick,nick);	
      if(host)
	infostd_dyn_str_strcpy(irce->host,host);	
      if(target)
	infostd_dyn_str_strcpy(irce->channel,target);	
      if(target2)
	infostd_dyn_str_strcpy(irce->target,target2);	
      if(text)
	infostd_dyn_str_strcpy(irce->text,text);	

    } else if(!strcmp(type,"353")) { /* result of names command */
      irce->type = NAMES;

      if(host)
	infostd_dyn_str_strcpy(irce->host,host);	
      if(target2)
	infostd_dyn_str_strcpy(irce->channel,target2);	
      if(text)
	infostd_dyn_str_strcpy(irce->text,text);	

    } else if(!strcmp(type,"TOPIC")) {
      /* result of setting the topic */
      irce->type = TOPIC;

      if(nick)
	infostd_dyn_str_strcpy(irce->nick,nick);	
      if(host)
	infostd_dyn_str_strcpy(irce->host,host);	
      if(target)
	infostd_dyn_str_strcpy(irce->channel,target);	
      if(text)
	infostd_dyn_str_strcpy(irce->text,text);	

    } else if(!strcmp(type,"332")) { 
      /* result of getting the topic */
      irce->type = TOPIC;

      if(nick)
	infostd_dyn_str_strcpy(irce->nick,nick);	
      if(host)
	infostd_dyn_str_strcpy(irce->host,host);	
      if(target2)
	infostd_dyn_str_strcpy(irce->channel,target2);	
      if(text)
	infostd_dyn_str_strcpy(irce->text,text);	    
    } else if(!strcmp(type,"333")) { 
      /* result of getting the topic - who set it */
      irce->type = TOPIC_WHO;

      if(nick)
	infostd_dyn_str_strcpy(irce->nick,nick);	
      if(host)
	infostd_dyn_str_strcpy(irce->host,host);	
      if(target)
	infostd_dyn_str_strcpy(irce->target,target);	
      if(target2)
	infostd_dyn_str_strcpy(irce->channel,target2);	
      if(text)
	infostd_dyn_str_strcpy(irce->text,text);	    
    } else if(!strcmp(type,"301")) { 
      /* A user is away */
      irce->type = IS_AWAY;

      if(host)
	infostd_dyn_str_strcpy(irce->host,host);	
      if(target2)
	infostd_dyn_str_strcpy(irce->target,target2);	
      if(text)
	infostd_dyn_str_strcpy(irce->text,text);	
    } else if(!strcmp(type,"303")) { 
      /* We are now back */
      irce->type = ISON;

      if(host)
	infostd_dyn_str_strcpy(irce->host,host);	
      if(target2)
	infostd_dyn_str_strcpy(irce->target,target2);	
      if(text)
	infostd_dyn_str_strcpy(irce->text,text);	
    } else if(!strcmp(type,"305")) { 
      /* We are now back */
      irce->type = BACK;

      if(host)
	infostd_dyn_str_strcpy(irce->host,host);	
      if(target2)
	infostd_dyn_str_strcpy(irce->target,target2);	
      if(text)
	infostd_dyn_str_strcpy(irce->text,text);	
    } else if(!strcmp(type,"306")) { 
      /* We are now away */
      irce->type = AWAY;

      if(host)
	infostd_dyn_str_strcpy(irce->host,host);	
      if(target2)
	infostd_dyn_str_strcpy(irce->target,target2);	
      if(text)
	infostd_dyn_str_strcpy(irce->text,text);	
    } else if(!strcmp(type,"365") || !strcmp(type,"366") ||
	      !strcmp(type,"368") || !strcmp(type,"374") ||
	      !strcmp(type,"376") || !strcmp(type,"394")) {
      /* end of messages - ignore! */
      irc_event_free(irce);
      return NULL;
    } else if(dtype > 400 && dtype < 503) {
      /* Generic error */
      irce->type = ERROR;

      if(host)
	infostd_dyn_str_strcpy(irce->host,host);	
      if(target2)
	infostd_dyn_str_strcpy(irce->target,target2);	
      if(text)
	infostd_dyn_str_strcpy(irce->text,text);	

    } else if(dtype >= 200 && dtype <= 600) {
      /* Numeric reply of some kind */
      irce->type = SYSTEM;

      if(nick)
	infostd_dyn_str_strcpy(irce->nick,nick);	
      if(host)
	infostd_dyn_str_strcpy(irce->host,host);	
      if(target)
	infostd_dyn_str_strcpy(irce->target,target);	

      if(text) {
	//snprintf(irce->text,1023,"IRC(%d) %s",dtype,text);
	//infostd_dyn_str_printf(irce->text, "[%3.3d] %s", dtype, text);
	infostd_dyn_str_printf(irce->text, "[%d] %s", dtype, text);
      } else {
	infostd_dyn_str_printf(irce->text,"IRC(%d)",dtype);
      }
    } else { /* assume general notice */
      irce->type = SYSTEM;

      if(nick)
	infostd_dyn_str_strcpy(irce->nick,nick);	
      if(host)
	infostd_dyn_str_strcpy(irce->host,host);	
      if(target)
	infostd_dyn_str_strcpy(irce->target,target);	
      if(text)
	infostd_dyn_str_strcpy(irce->text,text);
    }
  } else { /* generic notice */
    type = etext;

    text = strchr(type,' ');
    if(text) {
      *text = '\0';
      text++;

      if(*text == ':')
	text++;

    } else {
      type = NULL;
      text = etext;
    }
    // Pick up the PING after the fact
    if(type && !strcmp(type,"PING")) {
      fflush(stdout);
      irce->type = PING;
      infostd_dyn_str_strcpy(irce->host,text);
    } else {
      irce->type = SYSTEM;          
      infostd_dyn_str_strcpy(irce->text,text);
    }
  }

  /* auto lowercase the channel */
  infostd_dyn_str_lower(irce->channel);

  /* remove trailing spaces from text */
  text = irce->text->buff;
  len = irce->text->length;
  for(i = len - 1; i >= 0; i--) {
    if(text[i] == ' ') {
      infostd_dyn_str_delchar_at(irce->text, i);
      len--;
    } else {
      break;
    }
  }

  return irce;
}

VCSI_OBJECT handle_ping_event(VCSI_CONTEXT vc, 
			      IRC_CONN ircc, 
			      IRC_EVENT irce) {
  
  if(!IRC_STATE_CHECK(ircc, IRC_STATE_CONNECTED))
    return vc->false;
  
  memset(ircc->send_buf, 0, IRCBUF);
  
  snprintf(ircc->send_buf, IRCBUF, "PONG :%s\n", irce->host->buff);
  
  send(ircc->skt, ircc->send_buf, strlen(ircc->send_buf), 0);
  
  //printf("DEBUG: I PONG'd\n");

  return vc->true;
}

int irc_raw_ping(IRC_CONN ircc) {

  if(ircc && IRC_STATE_CHECK(ircc, IRC_STATE_CONNECTED)) {
    memset(ircc->send_buf, 0, IRCBUF);
    snprintf(ircc->send_buf, IRCBUF, "PING %s\n", ircc->server->buff);
    send(ircc->skt, ircc->send_buf, strlen(ircc->send_buf), 0);
    return 1;
  }
  return 0;
}

VCSI_OBJECT handle_event(VCSI_CONTEXT vc, 
			 IRC_CONN ircc, 
			 IRC_EVENT irce) {

  VCSI_OBJECT res, func, vars, vals;
  VCSI_OBJECT channel, target, nick, host, text;

  res = vc->true; /* default value for handled event */
  func = vars = vals = channel = target = nick = host = text = NULL;

 event_handle:

  switch(irce->type) {
  case SYSTEM:
    func = ircc->on_system;
    break;
  case NOTICE:
    func = ircc->on_notice;
    break;
  case ERROR:
    func = ircc->on_error;
    break;
  case PING:
    //printf("DEBUG: SERVER PING\n");
    res = handle_ping_event(vc,ircc,irce);
    break;
  case PONG:
    //printf("DEBUG: SERVER PONG\n");
    /* We don't actually care what it returns as long as it does... */
    break;
  case PUBLIC:
    func = ircc->on_public;
    break;
  case MSG:
    if(irce->nick->length)
      infostd_dyn_str_strcpy(ircc->last_msg,irce->nick->buff);

    func = ircc->on_msg;
    break;
  case ACTION:
    func = ircc->on_action;
    break;
  case CTCP:
    func = ircc->on_ctcp;
    break;
  case CTCP_PING:
    //printf("DEBUG: CTCP_PING\n");
    func = ircc->on_ctcp_ping;
    break;
  case CTCP_VERSION:
    //printf("DEBUG: CTCP_VERSION\n");
    func = ircc->on_ctcp_version;
    break;
  case CONNECT:
    func = ircc->on_connect;
    break;
  case JOIN:
    func = ircc->on_join;
    break;
  case PART:
    func = ircc->on_part;
    break;
  case QUIT:
    func = ircc->on_quit;
    break;
  case DISCONNECT:
    func = ircc->on_disconnect;
    break;
  case NICK_CHANGE:
    func = ircc->on_nick_change;
    break;
  case NICK_TAKEN:
    func = ircc->on_nick_taken;
    break;
  case KICK:
    func = ircc->on_kick;
    break;
  case NAMES:
    func = ircc->on_names;
    break;
  case TOPIC:
    func = ircc->on_topic;
    break;
  case TOPIC_WHO:
    func = ircc->on_topic_who;
    break;
  case IS_AWAY:
    func = ircc->on_is_away;
    break;
  case AWAY:
    func = ircc->on_away;
    break;
  case BACK:
    func = ircc->on_back;
    break;
  case ISON:
    func = ircc->on_ison;
    break;

  default:
    res = vc->false; /* not an event we understand */;
  }
  
  // Evaluate the associated handler, if it exists
  if(func) {

    if(irce->channel->length)
      channel = make_raw_string(vc,irce->channel->buff);

    if(irce->target->length)
      target = make_raw_string(vc,irce->target->buff);
    
    if(irce->nick->length)
      nick = make_raw_string(vc,irce->nick->buff);
    
    if(irce->host->length)
      host = make_raw_string(vc,irce->host->buff);
    
    if(irce->text->length)
      text = make_raw_string(vc,irce->text->buff);
    
    func = make_list(vc,6,func,channel,target,nick,host,text);

    // Error fallback context should be set
    res=eval(vc,func,vc->the_global_env);
  }     

  // Handle the connect event and then jump back to output the system msg
  if(irce->type == CONNECT) {
    irce->type = SYSTEM;
    goto event_handle;
  }

  // Clear away if set
  if(irce->type == BACK) {
    ircc->away = 0;
    infostd_dyn_str_clear(ircc->away_msg);
    irc_status_push(ircc,STATUS_BACK);
  } else if(irce->type == AWAY) {
    irc_status_push(ircc,STATUS_AWAY);
  } 

  /* Events which pertain directly to this user:
     do some back-end processing */
  if(strcasecmp(irce->nick->buff,ircc->nick->buff) == 0) {

    switch(irce->type) {
    case JOIN:
      // Update channel list
      irc_status_push(ircc,STATUS_JOINED);
      break;
    case PART:
      // Update channel list
      irc_status_push(ircc,STATUS_PARTED);
      break;      
    case NICK_CHANGE:
      // Update connection nick
      infostd_dyn_str_strcpy(ircc->nick,irce->text->buff);
      irc_status_push(ircc,STATUS_NICK_CHANGED);
      break;
    case QUIT:
      // Force a shutdown on the socket
      irc_status_push(ircc,STATUS_QUIT);
      shutdown(ircc->skt,2);
      IRC_STATE_UNSET(ircc,IRC_STATE_CONNECTED);
      break;
    }
  }
  return res;
}

void fake_event(VCSI_CONTEXT vc,
		IRC_CONN ircc,
		IRC_EVENT_TYPE type,
		char *channel,
		char *target,
		char *source,
		char *hostmask,
		char *text) {

  IRC_EVENT irce;

  irce = irc_event_init();

  irce->type = type;

  if(channel)
    infostd_dyn_str_strcpy(irce->channel,channel);	
  if(target)
    infostd_dyn_str_strcpy(irce->target,target);	
  if(source)
    infostd_dyn_str_strcpy(irce->nick,source);	
  if(hostmask)
    infostd_dyn_str_strcpy(irce->host,hostmask);	
  if(text)
    infostd_dyn_str_strcpy(irce->text,text);	

  handle_event(vc,ircc,irce);

  irc_event_free(irce);
}

IRC_STATUS irc_status_pop(IRC_CONN ircc) {
  if(ircc && IRC_STATE_CHECK(ircc,IRC_STATE_SQUEUE))
    return (IRC_STATUS) infostd_cqueue_pop(ircc->squeue);
  return STATUS_NONE;
}

void irc_status_push(IRC_CONN ircc, IRC_STATUS s) {
  if(ircc && IRC_STATE_CHECK(ircc,IRC_STATE_SQUEUE))
    infostd_cqueue_push(ircc->squeue, (char) s);
}

///////////////////////////////////////////////////////////////////////////
// IRC Functions 

IRC_CONN irc_connection_init(void) {
  IRC_CONN ircc = NULL;
  struct passwd* pd;
  time_t clock;
  int i;

  /* Create the Connection */
  ircc = MALLOC(SIZE_IRC_CONN);
  memset(ircc,0,SIZE_IRC_CONN);

  /* Initialize subparts of the Connection with some default values */
  ircc->server = infostd_dyn_str_init_with_str(IRC_DEFAULT_SERVER);
  ircc->port = IRC_DEFAULT_PORT;

  ircc->nick = infostd_dyn_str_init_with_str(IRC_DEFAULT_NICK);
  ircc->name = infostd_dyn_str_init_with_str(IRC_DEFAULT_NAME);

  ircc->idle = time(&clock);

  ircc->squeue = infostd_cqueue_init(100);

  ircc->away_msg = infostd_dyn_str_init(IRC_DYNSTR_DEFAULT);

  ircc->last_msg = infostd_dyn_str_init(IRC_DYNSTR_DEFAULT);
  
  ircc->current_chan = infostd_dyn_str_init(IRC_DYNSTR_DEFAULT);

  ircc->channel_list = infostd_dyn_strs_init(IRC_DYNSTR_DEFAULT);

  ircc->nick_list = infostd_dyn_strs_init(IRC_DYNSTR_DEFAULT);

  /* Attempt to look up better values for Nick and Name */
  if(getenv("USER") != NULL && (pd = getpwnam(getenv("USER"))) != NULL) {
    infostd_dyn_str_strcpy(ircc->nick, pd->pw_name);
    infostd_dyn_str_strcpy(ircc->name, pd->pw_gecos);
    for(i=0; i < ircc->name->length; i++) {
      if(ircc->name->buff[i] == ',') {
	infostd_dyn_str_kill_from(ircc->name, i);
	break;
      }
    }
  }
  return ircc;
}

void irc_connection_free(IRC_CONN ircc) {
  if(ircc) { 
    if(ircc->server)
      infostd_dyn_str_free(ircc->server);

    if(ircc->nick)
      infostd_dyn_str_free(ircc->nick);
    if(ircc->name)
      infostd_dyn_str_free(ircc->name);
   
    if(ircc->away_msg)
      infostd_dyn_str_free(ircc->away_msg);

    if(ircc->squeue)
      infostd_cqueue_free(ircc->squeue);

    if(ircc->last_msg)
      infostd_dyn_str_free(ircc->last_msg);

    if(ircc->current_chan)
      infostd_dyn_str_free(ircc->current_chan);

    if(ircc->channel_list)
      infostd_dyn_strs_free(ircc->channel_list);

    if(ircc->nick_list)
      infostd_dyn_strs_free(ircc->nick_list);

    FREE(ircc);
  }
}

VCSI_OBJECT error_irc_conn(VCSI_CONTEXT vc,
			   char *err,
			   VCSI_OBJECT e) {
  
  // When set, will error out
#ifdef IRC_NOT_CONNECTED_ERROR
  return error(vc, err, e);
#else // Otherwise, just return false
  return vc->false;
#endif
}

VCSI_OBJECT init_irc_conn(VCSI_CONTEXT vc) {
  
  IRC_CONN ircc = NULL;
  VCSI_OBJECT tmp;
  
  ircc = irc_connection_init();
  tmp = make_user_def(vc, (void*) ircc, irc_conn_type);

  user_def_set_eval(vc, tmp, PROCOPT, eval_irc_conn);  

  return tmp;
}

VCSI_OBJECT connect_irc_conn(VCSI_CONTEXT vc, 
			     VCSI_OBJECT x) {

  int s, len;
  struct hostent *hp;
  struct sockaddr_in name;
  
  if(!same_type_user_def(vc, x, irc_conn_type))
    return error(vc, "objected passed to irc-connect is of the wrong type", x);
  
  if((hp = gethostbyname(IRCC(x)->server->buff)) == NULL)
    return error_irc_conn(vc, "unknown host", make_raw_string(vc, IRCC(x)->server->buff));
  
  if((s = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    return error_irc_conn(vc, "unable to create socket", make_raw_string(vc, IRCC(x)->server->buff));
  
  memset(&name, 0, sizeof(struct sockaddr_in));
  name.sin_family = AF_INET;
  name.sin_port = htons(IRCC(x)->port);
  memcpy(&name.sin_addr, hp->h_addr_list[0], hp->h_length);
  len = sizeof(struct sockaddr_in);
  
  if(connect(s, (struct sockaddr *) &name, len) < 0)
    return error_irc_conn(vc, "connection refused", make_raw_string(vc, IRCC(x)->server->buff));
  
  IRCC(x)->skt = s;

  /* send the nick */
  memset(IRCC(x)->send_buf, 0, IRCBUF);

  snprintf(IRCC(x)->send_buf, IRCBUF, "NICK %s\n", IRCC(x)->nick->buff);

  send(IRCC(x)->skt, IRCC(x)->send_buf, strlen(IRCC(x)->send_buf), 0);

  /* send the user */
  memset(IRCC(x)->send_buf, 0, IRCBUF);

  snprintf(IRCC(x)->send_buf, IRCBUF, "USER %s %s %s :%s\n", IRCC(x)->nick->buff,
	   "localhost", "localhost", IRCC(x)->name->buff);

  send(IRCC(x)->skt, IRCC(x)->send_buf, strlen(IRCC(x)->send_buf), 0);

  IRC_STATE_SET(IRCC(x), IRC_STATE_CONNECTED);

  return vc->true;
}

VCSI_OBJECT disconnect_irc_conn(VCSI_CONTEXT vc, 
				VCSI_OBJECT x) {
  
  if(!same_type_user_def(vc, x, irc_conn_type))
    return error(vc,"objected passed to irc-connect is of the wrong type", x);

  if(IRC_STATE_CHECK(IRCC(x), IRC_STATE_CONNECTED)) {
    
    shutdown(IRCC(x)->skt, 2);
    close(IRCC(x)->skt);
    
    IRC_STATE_UNSET(IRCC(x), IRC_STATE_CONNECTED);
  }
  return vc->true;
}

VCSI_OBJECT enable_status_queue_irc_conn(VCSI_CONTEXT vc, 
					 VCSI_OBJECT x) {

  if(!same_type_user_def(vc, x, irc_conn_type))
    return error(vc,"objected passed to irc-status-queue-enable is of the wrong type", x);

  IRC_STATE_SET(IRCC(x), IRC_STATE_SQUEUE);

  return vc->true;
}

VCSI_OBJECT disable_status_queue_irc_conn(VCSI_CONTEXT vc, 
					  VCSI_OBJECT x) {

  if(!same_type_user_def(vc, x, irc_conn_type))
    return error(vc,"objected passed to irc-status-queue-disable is of the wrong type", x);

  IRC_STATE_UNSET(IRCC(x), IRC_STATE_SQUEUE);

  return vc->true;
}

VCSI_OBJECT pop_status_queue_irc_conn(VCSI_CONTEXT vc, 
				      VCSI_OBJECT x) {

  IRC_STATUS i;
  
  if(!same_type_user_def(vc, x, irc_conn_type))
    return error(vc,"objected passed to irc-connect is of the wrong type", x);

  i = irc_status_pop(IRCC(x));

  switch(i) {
  case STATUS_IDLE:
    return make_symbol(vc, "idle");
    break;
  case STATUS_NOT_IDLE:
    return make_symbol(vc, "not-idle");
    break;
  case STATUS_AWAY:
    return make_symbol(vc, "away");
    break;
  case STATUS_BACK:
    return make_symbol(vc, "back");
    break;
  case STATUS_QUIT:
    return make_symbol(vc, "quit");
    break;
  case STATUS_NICK_CHANGED:
    return make_symbol(vc, "nick-change");
    break;
  case STATUS_JOINED:
    return make_symbol(vc, "joined");
    break;
  case STATUS_PARTED:
    return make_symbol(vc, "parted");
    break;
  default:
    return make_symbol(vc, "none");
    break;    
  }
  return vc->false;
}

VCSI_OBJECT raw_irc_conn(VCSI_CONTEXT vc, 
			 VCSI_OBJECT x, 
			 VCSI_OBJECT y) {

  if(!same_type_user_def(vc, x, irc_conn_type))
    return error(vc,"objected passed to irc-raw is of the wrong type", x);

  check_arg_type(vc, y, STRING, "irc-raw");

  if(!IRC_STATE_CHECK(IRCC(x), IRC_STATE_CONNECTED))
    return error_irc_conn(vc,"not connected", x);
  
  memset(IRCC(x)->send_buf, 0, IRCBUF);

  snprintf(IRCC(x)->send_buf, IRCBUF, "%s\n", STR(y));

  send(IRCC(x)->skt, IRCC(x)->send_buf, strlen(IRCC(x)->send_buf), 0);

  return vc->true;
}

VCSI_OBJECT quit_irc_conn(VCSI_CONTEXT vc, 
			  VCSI_OBJECT x, 
			  VCSI_OBJECT y) {

  if(!same_type_user_def(vc, x, irc_conn_type))
    return error(vc, "objected passed to irc-quit is of the wrong type", x);

  if(!IRC_STATE_CHECK(IRCC(x), IRC_STATE_CONNECTED)) {
    irc_status_push(IRCC(x), STATUS_QUIT);
    return vc->true;
  }
  
  check_arg_type(vc, y, STRING, "irc-quit");

  /* send the command */
  memset(IRCC(x)->send_buf, 0, IRCBUF);

  snprintf(IRCC(x)->send_buf, IRCBUF, "QUIT :%s\n", STR(y));

  send(IRCC(x)->skt, IRCC(x)->send_buf, strlen(IRCC(x)->send_buf), 0);

  return vc->true;
}

VCSI_OBJECT join_irc_conn(VCSI_CONTEXT vc, 
			  VCSI_OBJECT x, 
			  VCSI_OBJECT y) {

  if(!same_type_user_def(vc, x, irc_conn_type))
    return error(vc, "objected passed to irc-join is of the wrong type", x);

  check_arg_type(vc, y, STRING, "irc-join");

  if(!IRC_STATE_CHECK(IRCC(x), IRC_STATE_CONNECTED))
    return error_irc_conn(vc, "not connected", x);
  
  /* send the command */
  memset(IRCC(x)->send_buf, 0, IRCBUF);

  snprintf(IRCC(x)->send_buf, IRCBUF, "JOIN %s\n", STR(y));

  send(IRCC(x)->skt, IRCC(x)->send_buf, strlen(IRCC(x)->send_buf), 0);

  // Save the Channel
  infostd_dyn_str_strcpy(IRCC(x)->current_chan, STR(y));

  return vc->true;
}

VCSI_OBJECT part_irc_conn(VCSI_CONTEXT vc, 
			  VCSI_OBJECT x, 
			  VCSI_OBJECT y) {

  if(!same_type_user_def(vc, x, irc_conn_type))
    return error(vc, "objected passed to irc-part is of the wrong type", x);

  check_arg_type(vc, y, STRING, "irc-part");

  if(!IRC_STATE_CHECK(IRCC(x), IRC_STATE_CONNECTED))
    return error_irc_conn(vc, "not connected", x);

  /* send the command */
  memset(IRCC(x)->send_buf, 0, IRCBUF);

  snprintf(IRCC(x)->send_buf, IRCBUF, "PART %s\n", STR(y));

  send(IRCC(x)->skt, IRCC(x)->send_buf, strlen(IRCC(x)->send_buf), 0);

  return vc->true;
}

VCSI_OBJECT nick_irc_conn(VCSI_CONTEXT vc, 
			  VCSI_OBJECT x, 
			  VCSI_OBJECT y) {

  if(!same_type_user_def(vc, x, irc_conn_type))
    return error(vc, "objected passed to irc-nick is of the wrong type", x);

  check_arg_type(vc, y, STRING, "irc-nick");

  // Assume we get to keep it
  infostd_dyn_str_strcpy(IRCC(x)->nick, STR(y));
  irc_status_push(IRCC(x), STATUS_NICK_CHANGED);

  if(!IRC_STATE_CHECK(IRCC(x), IRC_STATE_CONNECTED)) {
    return vc->true;
  }

  /* send the command */
  memset(IRCC(x)->send_buf, 0, IRCBUF);

  snprintf(IRCC(x)->send_buf, IRCBUF, "NICK %s\n", STR(y));

  send(IRCC(x)->skt, IRCC(x)->send_buf, strlen(IRCC(x)->send_buf), 0);
  
  return vc->true;
}

VCSI_OBJECT name_irc_conn(VCSI_CONTEXT vc, 
			  VCSI_OBJECT x, 
			  VCSI_OBJECT y) {

  if(!same_type_user_def(vc, x, irc_conn_type))
    return error(vc, "objected passed to irc-name is of the wrong type", x);

  check_arg_type(vc, y, STRING, "irc-name");

  infostd_dyn_str_strcpy(IRCC(x)->name, STR(y));
  
  return vc->true;
}

VCSI_OBJECT server_irc_conn(VCSI_CONTEXT vc, 
			    VCSI_OBJECT x, 
			    VCSI_OBJECT y) {

  if(!same_type_user_def(vc, x, irc_conn_type))
    return error(vc, "objected passed to irc-server is of the wrong type", x);

  check_arg_type(vc, y, STRING, "irc-server");

  if(IRC_STATE_CHECK(IRCC(x), IRC_STATE_CONNECTED)) {
    return vc->false;
  }

  infostd_dyn_str_strcpy(IRCC(x)->server, STR(y));
  
  return vc->true;
}

VCSI_OBJECT port_irc_conn(VCSI_CONTEXT vc, 
			  VCSI_OBJECT x, 
			  VCSI_OBJECT y) {

  if(!same_type_user_def(vc, x, irc_conn_type))
    return error(vc, "objected passed to irc-port is of the wrong type", x);

  check_arg_type(vc, y, LNGNUM, "irc-port");

  if(IRC_STATE_CHECK(IRCC(x), IRC_STATE_CONNECTED)) {
    return vc->false;
  }

  IRCC(x)->port = LNGN(y);
  
  return vc->true;
}


VCSI_OBJECT notice_irc_conn(VCSI_CONTEXT vc, 
			    VCSI_OBJECT x, 
			    VCSI_OBJECT y, 
			    VCSI_OBJECT z) {

  if(!same_type_user_def(vc, x, irc_conn_type))
    return error(vc, "objected passed to irc-notice is of the wrong type", x);

  check_arg_type(vc, y, STRING, "irc-notice");
  check_arg_type(vc, z, STRING, "irc-notice");

  if(!IRC_STATE_CHECK(IRCC(x), IRC_STATE_CONNECTED))
    return error_irc_conn(vc, "not connected", x);
  
  /* send the command */
  memset(IRCC(x)->send_buf, 0 ,IRCBUF);

  snprintf(IRCC(x)->send_buf, IRCBUF, "NOTICE %s :%s\n", STR(y), STR(z));

  send(IRCC(x)->skt, IRCC(x)->send_buf, strlen(IRCC(x)->send_buf), 0);

  irc_reset_idle(IRCC(x));

  return vc->true;
}

VCSI_OBJECT privmsg_irc_conn(VCSI_CONTEXT vc, 
			     VCSI_OBJECT x, 
			     VCSI_OBJECT y, 
			     VCSI_OBJECT z) {

  if(!same_type_user_def(vc, x, irc_conn_type))
    return error(vc, "objected passed to irc-privmsg is of the wrong type", x);

  check_arg_type(vc, y, STRING, "irc-privmsg");
  check_arg_type(vc, z, STRING, "irc-privmsg");

  if(!IRC_STATE_CHECK(IRCC(x), IRC_STATE_CONNECTED))
    return error_irc_conn(vc, "not connected", x);
  
  /* send the command */
  memset(IRCC(x)->send_buf, 0, IRCBUF);

  snprintf(IRCC(x)->send_buf, IRCBUF, "PRIVMSG %s :%s\n", STR(y), STR(z));

  send(IRCC(x)->skt, IRCC(x)->send_buf, strlen(IRCC(x)->send_buf), 0);

  if(STR(y)[0] != '#') infostd_dyn_str_strcpy(IRCC(x)->last_msg, STR(y));

  irc_reset_idle(IRCC(x));

  return vc->true;
}

VCSI_OBJECT me_irc_conn(VCSI_CONTEXT vc, 
			VCSI_OBJECT x, 
			VCSI_OBJECT y, 
			VCSI_OBJECT z) {

  if(!same_type_user_def(vc, x, irc_conn_type))
    return error(vc, "objected passed to irc-me is of the wrong type", x);

  check_arg_type(vc, y, STRING, "irc-me");
  check_arg_type(vc, z, STRING, "irc-me");

  if(!IRC_STATE_CHECK(IRCC(x), IRC_STATE_CONNECTED))
    return error_irc_conn(vc, "not connected", x);
  
  /* send the command */
  memset(IRCC(x)->send_buf, 0, IRCBUF);

  snprintf(IRCC(x)->send_buf, IRCBUF, "PRIVMSG %s :%cACTION %s%c\n",
	   STR(y), 1, STR(z), 1);

  send(IRCC(x)->skt, IRCC(x)->send_buf, strlen(IRCC(x)->send_buf), 0);

  irc_reset_idle(IRCC(x));

  return vc->true;
}

VCSI_OBJECT ctcp_irc_conn(VCSI_CONTEXT vc, 
			  VCSI_OBJECT x, 
			  VCSI_OBJECT y, 
			  VCSI_OBJECT z) {
  
  if(!same_type_user_def(vc, x, irc_conn_type))
    return error(vc, "objected passed to irc-ctcp is of the wrong type", x);

  check_arg_type(vc, y, STRING, "irc-ctcp");
  check_arg_type(vc, z, STRING, "irc-ctcp");

  if(!IRC_STATE_CHECK(IRCC(x), IRC_STATE_CONNECTED))
    return error_irc_conn(vc, "not connected", x);
  
  /* send the command */
  memset(IRCC(x)->send_buf, 0, IRCBUF);

  snprintf(IRCC(x)->send_buf, IRCBUF, "PRIVMSG %s :%c%s%c\n",
	   STR(y), 1, STR(z), 1);

  send(IRCC(x)->skt, IRCC(x)->send_buf, strlen(IRCC(x)->send_buf), 0);

  return vc->true;
}

VCSI_OBJECT topic_irc_conn(VCSI_CONTEXT vc, 
			   VCSI_OBJECT args, 
			   int length) {

  VCSI_OBJECT x, y, z;
  
  x = y = z = NULL;
  
  if(length < 2 || length > 3)
    return error(vc, "invalid number of arguments to irc-topic", args);

  x = car(vc, args);
  if(!same_type_user_def(vc, x, irc_conn_type))
    return error(vc, "objected passed to irc-topic is of the wrong type", x);

  y = cadr(vc, args);
  check_arg_type(vc, y, STRING, "irc-topic");

  
  if(length == 3) {
    z = caddr(vc, args);
    check_arg_type(vc, z, STRING, "irc-topic");
  } 

  if(!IRC_STATE_CHECK(IRCC(x), IRC_STATE_CONNECTED))
    return error_irc_conn(vc, "not connected", x);

  /* send the command */
  memset(IRCC(x)->send_buf, 0, IRCBUF);

  if(z)
    snprintf(IRCC(x)->send_buf, IRCBUF, "TOPIC %s :%s\n", STR(y), STR(z));
  else
    snprintf(IRCC(x)->send_buf, IRCBUF, "TOPIC %s\n", STR(y));

  send(IRCC(x)->skt, IRCC(x)->send_buf, strlen(IRCC(x)->send_buf), 0);

  return vc->true;
}

VCSI_OBJECT names_irc_conn(VCSI_CONTEXT vc,
			   VCSI_OBJECT args,
			   int length) {
  VCSI_OBJECT x, y;
  
  x = y = NULL;
  
  if(length < 1 || length > 2)
    return error(vc, "invalid number of arguments to irc-names", args);

  x = car(vc, args);
  if(!same_type_user_def(vc, x, irc_conn_type))
    return error(vc, "objected passed to irc-names is of the wrong type", x);

  if(!IRC_STATE_CHECK(IRCC(x), IRC_STATE_CONNECTED))
    return error_irc_conn(vc, "not connected", x);

  if(length == 2) {
    y = cadr(vc, args);
    check_arg_type(vc, y, STRING, "irc-names");
  }

  /* send the command */
  memset(IRCC(x)->send_buf, 0, IRCBUF);

  if(y)
    snprintf(IRCC(x)->send_buf, IRCBUF, "NAMES %s\n", STR(y));
  else
    snprintf(IRCC(x)->send_buf, IRCBUF, "NAMES\n");

  send(IRCC(x)->skt, IRCC(x)->send_buf, strlen(IRCC(x)->send_buf), 0);

  return vc->true;

}

VCSI_OBJECT kick_irc_conn(VCSI_CONTEXT vc, 
			  VCSI_OBJECT args, 
			  int length) {
  VCSI_OBJECT c, x, y, z;

  c = x = y = z = NULL;

  if(length < 3 || length > 4)
    return error(vc, "invalid number of arguments to irc-kick", args);

  c = car(vc, args); /* irc-conn */
  if(!same_type_user_def(vc, c, irc_conn_type))
    return error(vc, "objected passed to irc-kick is of the wrong type", x);

  if(!IRC_STATE_CHECK(IRCC(c), IRC_STATE_CONNECTED))
    return error_irc_conn(vc, "not connected", c);

  x = cadr(vc, args); /* channel */
  check_arg_type(vc, x, STRING, "irc-kick");

  y = caddr(vc, args); /* nick */
  check_arg_type(vc, y, STRING, "irc-kick");
  
  // You can't kick yourself
  if(!strcasecmp(STR(y), IRCC(c)->nick->buff))
    return vc->false;

  if(length == 4) {
    z = cadddr(vc, args);
    check_arg_type(vc, z, STRING, "irc-kick");
  }

  /* send the command */
  memset(IRCC(c)->send_buf, 0, IRCBUF);
  
  if(z)
    snprintf(IRCC(c)->send_buf, IRCBUF, "KICK %s %s :%s\n",
	     STR(x), STR(y), STR(z));
  else
    snprintf(IRCC(c)->send_buf, IRCBUF, "KICK %s %s :%s\n",
	     STR(x), STR(y), IRCC(c)->nick->buff);
  
  send(IRCC(c)->skt, IRCC(c)->send_buf, strlen(IRCC(c)->send_buf), 0);

  return vc->true;
}

VCSI_OBJECT away_irc_conn(VCSI_CONTEXT vc,
			  VCSI_OBJECT args,
			  int length) {
  VCSI_OBJECT x, y;

  x = y = NULL;
  
  if(length < 1 || length > 2)
    return error(vc, "invalid number of arguments to irc-away", args);

  x = car(vc, args);
  if(!same_type_user_def(vc, x, irc_conn_type))
    return error(vc, "objected passed to irc-away is of the wrong type", x);

  if(!IRC_STATE_CHECK(IRCC(x), IRC_STATE_CONNECTED))
    return error_irc_conn(vc, "not connected", x);

  if(length == 2) {
    y = cadr(vc, args);
    check_arg_type(vc, y, STRING, "irc-away");
  }

  /* send the command */
  memset(IRCC(x)->send_buf, 0, IRCBUF);

  if(y) {
    snprintf(IRCC(x)->send_buf, IRCBUF, "AWAY :%s\n" ,STR(y));
    infostd_dyn_str_strcpy(IRCC(x)->away_msg, STR(y));
  } else
    snprintf(IRCC(x)->send_buf, IRCBUF, "AWAY :away\n");

  send(IRCC(x)->skt, IRCC(x)->send_buf, strlen(IRCC(x)->send_buf), 0);

  irc_set_away(IRCC(x));

  return vc->true;

}

VCSI_OBJECT away_msg_irc_conn(VCSI_CONTEXT vc, 
			      VCSI_OBJECT x) {	       

  if(!same_type_user_def(vc, x, irc_conn_type))
    return error(vc, "objected passed to irc-back is of the wrong type", x);
  
  return make_raw_string(vc, IRCC(x)->away_msg->buff);
}


VCSI_OBJECT back_irc_conn(VCSI_CONTEXT vc, 
			  VCSI_OBJECT x) {	       

  if(!same_type_user_def(vc, x, irc_conn_type))
    return error(vc, "objected passed to irc-back is of the wrong type", x);
  
  if(!IRC_STATE_CHECK(IRCC(x), IRC_STATE_CONNECTED))
    return error_irc_conn(vc, "not connected", x);

  /* send the command */
  memset(IRCC(x)->send_buf, 0, IRCBUF);

  snprintf(IRCC(x)->send_buf, IRCBUF, "AWAY\n");

  send(IRCC(x)->skt, IRCC(x)->send_buf, strlen(IRCC(x)->send_buf), 0);

  return vc->true;
}

VCSI_OBJECT ison_irc_conn(VCSI_CONTEXT vc, 
			  VCSI_OBJECT x, 
			  VCSI_OBJECT y) {

  if(!same_type_user_def(vc, x, irc_conn_type))
    return error(vc, "objected passed to irc-ison is of the wrong type", x);
  
  check_arg_type(vc, y, STRING, "irc-ison");
  
  if(!IRC_STATE_CHECK(IRCC(x), IRC_STATE_CONNECTED))
    return error_irc_conn(vc, "not connected", x);

  /* send the command */
  memset(IRCC(x)->send_buf, 0, IRCBUF);

  snprintf(IRCC(x)->send_buf, IRCBUF, "ISON %s\n", STR(y));

  send(IRCC(x)->skt, IRCC(x)->send_buf, strlen(IRCC(x)->send_buf), 0);

  return vc->true;
}

VCSI_OBJECT ping_reply_irc_conn(VCSI_CONTEXT vc, 
				VCSI_OBJECT x, 
				VCSI_OBJECT y,
				VCSI_OBJECT z) {

  if(!same_type_user_def(vc, x, irc_conn_type))
    return error(vc, "objected passed to irc-ping-reply is of the wrong type", x);
  
  check_arg_type(vc, y, STRING, "irc-ping-reply");
  check_arg_type(vc, z, STRING, "irc-ping-reply");

  if(!IRC_STATE_CHECK(IRCC(x), IRC_STATE_CONNECTED))
    return error_irc_conn(vc, "not connected", x);

  /* send the command */
  memset(IRCC(x)->send_buf, 0, IRCBUF);

  snprintf(IRCC(x)->send_buf, IRCBUF, "NOTICE %s :%cPING %s%c\n",
	   STR(y), 1, STR(z), 1);

  send(IRCC(x)->skt, IRCC(x)->send_buf, strlen(IRCC(x)->send_buf), 0);

  return vc->true;
}

VCSI_OBJECT version_reply_irc_conn(VCSI_CONTEXT vc, 
				   VCSI_OBJECT x, 
				   VCSI_OBJECT y,
				   VCSI_OBJECT z) {
  
  if(!same_type_user_def(vc, x, irc_conn_type))
    return error(vc, "objected passed to irc-version-reply is of the wrong type", x);

  check_arg_type(vc, y, STRING, "irc-version-reply");
  check_arg_type(vc, z, STRING, "irc-version-reply");
  
  if(!IRC_STATE_CHECK(IRCC(x), IRC_STATE_CONNECTED))
    return error_irc_conn(vc, "not connected", x);
  
  /* send the command */
  memset(IRCC(x)->send_buf, 0, IRCBUF);

  snprintf(IRCC(x)->send_buf, IRCBUF, "NOTICE %s :%cVERSION %s%c\n",
	   STR(y), 1, STR(z), 1);

  send(IRCC(x)->skt, IRCC(x)->send_buf, strlen(IRCC(x)->send_buf), 0);

  return vc->true;
}

///////////////////////////////////////////////////////////////////////////
// Event Handlers

VCSI_OBJECT set_handler_irc_conn(VCSI_CONTEXT vc, 
				 VCSI_OBJECT x, 
				 VCSI_OBJECT y, 
				 VCSI_OBJECT z) {

  char *h;

  if(!same_type_user_def(vc, x, irc_conn_type))
    return error(vc, "objected passed to irc-handler-set! is of the wrong type", x);
  
  if(!(TYPEP(y, SYMBOL) || TYPEP(y, STRING)))
    error(vc, "invalid type to irc-handler-set!", y);

  h = (TYPEP(y, SYMBOL)) ? SNAME(y) : STR(y);
  
  if(!strcasecmp(h, "notice")) {
    IRCC(x)->on_notice = z;
  } else if(!strcasecmp(h, "system")) {
    IRCC(x)->on_system = z;
  } else if(!strcasecmp(h, "error")) {
    IRCC(x)->on_error = z;
  } else if(!strcasecmp(h, "public")) {
    IRCC(x)->on_public = z;
  } else if(!strcasecmp(h, "msg")) {
    IRCC(x)->on_msg = z;
  } else if(!strcasecmp(h, "action")) {
    IRCC(x)->on_action = z;
  } else if(!strcasecmp(h, "ctcp")) {
    IRCC(x)->on_ctcp = z;
  } else if(!strcasecmp(h, "ping")) {
    IRCC(x)->on_ctcp_ping = z;
  } else if(!strcasecmp(h, "version")) {
    IRCC(x)->on_ctcp_version = z;
  } else if(!strcasecmp(h, "connect")) {
    IRCC(x)->on_connect = z;
  } else if(!strcasecmp(h, "join")) {
    IRCC(x)->on_join = z;
  } else if(!strcasecmp(h, "part")) {
    IRCC(x)->on_part = z;
  } else if(!strcasecmp(h, "quit")) {
    IRCC(x)->on_quit = z;
  } else if(!strcasecmp(h, "disconnect")) {
    IRCC(x)->on_disconnect = z;
  } else if(!strcasecmp(h, "nick-change")) {
    IRCC(x)->on_nick_change = z;
  } else if(!strcasecmp(h, "nick-taken")) {
    IRCC(x)->on_nick_taken = z;
  } else if(!strcasecmp(h, "kick")) {
    IRCC(x)->on_kick = z;
  } else if(!strcasecmp(h, "names")) {
    IRCC(x)->on_names = z;
  } else if(!strcasecmp(h, "topic")) {
    IRCC(x)->on_topic = z;
  } else if(!strcasecmp(h, "topic-who")) {
    IRCC(x)->on_topic_who = z;
  } else if(!strcasecmp(h, "is-away")) {
    IRCC(x)->on_is_away = z;
  } else if(!strcasecmp(h, "away")) {
    IRCC(x)->on_away = z;
  } else if(!strcasecmp(h, "back")) {
    IRCC(x)->on_back = z;
  } else if(!strcasecmp(h, "ison")) {
    IRCC(x)->on_ison = z;
  } else 
    return error(vc, "invalid handler name", y);

  return vc->true;
}

VCSI_OBJECT handle_one_event_irc_conn(VCSI_CONTEXT vc, 
				      VCSI_OBJECT x) {

  VCSI_OBJECT res = vc->true;
  IRC_EVENT irce = NULL;
  struct timeval tv;
  unsigned char c;
  fd_set rfds;
  long i;
  int n;


  if(!same_type_user_def(vc, x, irc_conn_type))
    return error(vc, "objected passed to irc-handle-one-event is of the wrong type", x);

  if(!IRC_STATE_CHECK(IRCC(x), IRC_STATE_CONNECTED))
    return error_irc_conn(vc, "not connected", x);

  memset(IRCC(x)->recv_buf, 0, IRCBUF);

  /* Wait for up to five[three] minutes, then send a forced PING and deal with the result */
  tv.tv_sec = 30; /* Thirty Seconds */
  tv.tv_usec = 0;
    
  FD_ZERO(&rfds);
  FD_SET(IRCC(x)->skt, &rfds);

  if(!select(IRCC(x)->skt+1, &rfds, NULL, NULL, &tv)) {
    /* Nothing, send a keep-alive ping */
    memset(IRCC(x)->send_buf, 0, IRCBUF);
    
    snprintf(IRCC(x)->send_buf, IRCBUF, "PING %s\n", IRCC(x)->server->buff);
    //printf("DEBUG: %s", IRCC(x)->send_buf);
    send(IRCC(x)->skt, IRCC(x)->send_buf, strlen(IRCC(x)->send_buf), 0);
    
  }

  for(i=0, c=0; i < IRCBUF; i++) {
    n = recv(IRCC(x)->skt, &c, 1, 0);

    if(0 && n ==- 1) { // Unhelpful error messages
      /* Something went wrong, maybe */
      printf("DEBUG: Something went wrong on recv...\n");
      switch(errno) {
      case EBADF:  
	printf("DEBUG: The argument s is an invalid descriptor.");
	break;
      case ECONNREFUSED:
	printf("DEBUG: Connection refused.\n");
	break;
      case ENOTCONN:
	printf("DEBUG: Not connected.\n");
	break;
      case ENOTSOCK:
	printf("DEBUG: Argument skt is not a socket.\n");
	break;
      case EAGAIN:
	printf("DEBUG: Operation would block or timed out.\n");
	break;
      case EINTR:
	printf("DEBUG: Interrupt received.\n");
	break;
      case EFAULT:
	printf("DEBUG: Recieve buffer points outside address space.\n");
	break;
      case EINVAL:
	printf("DEBUG: Invalid argument passed.\n");
	break;
      default:
	printf("DEBUG: Not sure (%d).\n", errno);
      }
      break;
    }
    
    if(c == '\n')
      break;
    
    IRCC(x)->recv_buf[i] = c;
  }
  IRCC(x)->recv_buf[i]=0;
  
  if(n == -1) {
    IRC_STATE_UNSET(IRCC(x), IRC_STATE_CONNECTED);
    irce = parse_event(NULL);
  } else 
    irce = parse_event(IRCC(x)->recv_buf);
  
  if(irce) {
    res = handle_event(vc, IRCC(x), irce);
    irc_event_free(irce);
  }

  return res;
}

VCSI_OBJECT event_ready_irc_conn(VCSI_CONTEXT vc,
				 VCSI_OBJECT x) {

  struct timeval tv;
  fd_set rfds;
  int i;

  if(!same_type_user_def(vc, x, irc_conn_type))
    return error(vc, "objected passed to irc-event-ready? is of the wrong type", x);

  if(!IRC_STATE_CHECK(IRCC(x), IRC_STATE_CONNECTED))
    return vc->false;

  FD_ZERO(&rfds);
  FD_SET(IRCC(x)->skt, &rfds);
   
  tv.tv_sec = 0;
  tv.tv_usec = 0;
  i = select(IRCC(x)->skt+1, &rfds, NULL, NULL, &tv);
   
  if(i || i == -1) /* So a disconnect get triggered */
    return vc->true;
  
  return vc->false;
}

/* Parse raw string and execute commands
 * (irc-command-parse ircc "/join #sass")
 * (irc-command-parse ircc "#sass" "/me lala")
 */
VCSI_OBJECT command_parse_irc_conn(VCSI_CONTEXT vc,
				   VCSI_OBJECT args,
				   int length) {
  VCSI_OBJECT ircc, chan, s;
  char *ln, *ln2;
  char str[1024];

  if(length == 2) {
    ircc = car(vc, args);
    s = cadr(vc, args);
    chan = NULL;
    
    if(!same_type_user_def(vc, ircc, irc_conn_type))
      return error(vc, "objected passed to irc-command-parse is of the wrong type",
	    ircc);
    
    check_arg_type(vc, s, STRING, "irc-command-parse");
  } else if(length == 3) {
    ircc = car(vc, args);
    chan = cadr(vc, args);
    s = caddr(vc, args);

    if(!same_type_user_def(vc, ircc, irc_conn_type))
      return error(vc, "objected passed to irc-command-parse is of the wrong type",
	    ircc);

    check_arg_type(vc, chan, STRING, "irc-command-parse");    
    check_arg_type(vc, s, STRING, "irc-command-parse");
  } else 
    return error(vc, "improper arguments to irc-command-parse", args);

  memset(str, 0, 1024);
  strncpy(str, STR(s), 1024);
  
  if(!strcasecmp(str, "/connect")) {
    return connect_irc_conn(vc, ircc);
  } else if(!strcasecmp(str, "/disconnect")) {
    return disconnect_irc_conn(vc,ircc);
  } else if(!strncasecmp(str, "/join ", 6) && strlen(str) > 6) {
    ln = str + 6;
    return join_irc_conn(vc, ircc, make_raw_string(vc, ln));
  } else if(!strncasecmp(str, "/part ", 6) && strlen(str) > 6) {
    ln = str + 6;
    return part_irc_conn(vc, ircc, make_raw_string(vc, ln));
  } else if(!strcasecmp(str, "/part") && chan) {
    return part_irc_conn(vc, ircc, chan);
  } else if(!strncasecmp(str, "/nick ", 6) && strlen(str) > 6) {
    ln = str + 6;
    return nick_irc_conn(vc, ircc, make_raw_string(vc, ln));
  } else if(!strncasecmp(str, "/msg ", 5) && strlen(str) > 5) {
    ln = str + 5;
    if((ln2 = strchr(ln, ' '))) {
      *ln2 = '\0';
      ln2++;
      fake_event(vc, IRCC(ircc), MSG, NULL, ln, IRCC(ircc)->nick->buff, NULL, ln2);
      return privmsg_irc_conn(vc, ircc, make_raw_string(vc, ln), make_raw_string(vc, ln2));
    }
  } else if(!strncasecmp(str, "/ctcp ", 6) && strlen(str) > 6) {
    ln = str + 6;
    if((ln2 = strchr(ln, ' '))) {
      *ln2 = '\0';
      ln2++;
      return ctcp_irc_conn(vc, ircc, make_raw_string(vc, ln), make_raw_string(vc, ln2));
    }
  } else if(!strncasecmp(str, "/me ", 4)  && strlen(str) > 4 && chan) {
    ln = str + 4;
    fake_event(vc, IRCC(ircc), ACTION, STR(chan), NULL, IRCC(ircc)->nick->buff, NULL, ln);
    return me_irc_conn(vc, ircc, chan, make_raw_string(vc, ln));
  } else if(!strncasecmp(str, "/notice ", 8)  && strlen(str) > 8 && chan) {
    ln = str + 8;
    return notice_irc_conn(vc, ircc, chan, make_raw_string(vc, ln));
  } else if(!strncasecmp(str, "/raw ", 5) && strlen(str) > 5) {
    ln = str + 5;
    return raw_irc_conn(vc, ircc, make_raw_string(vc, ln));
  } else if(!strcasecmp(str, "/names")) {
    if(chan)
      names_irc_conn(vc, make_list(vc, 2, ircc, chan), 2);
    else
      names_irc_conn(vc,cons(vc, chan, NULL), 1);
  } else if(!strncasecmp(str, "/ison ", 6) && strlen(str) > 6) {
    ln = str + 6;
    return ison_irc_conn(vc, ircc, make_raw_string(vc, ln));
  } else if(!strncasecmp(str, "/away ", 6) && strlen(str) > 6) {
    ln = str + 6;
    return away_irc_conn(vc, make_list(vc, 2, ircc, make_raw_string(vc, ln)), 2);
  } else if(!strcasecmp(str, "/away")) {
    return away_irc_conn(vc, cons(vc, ircc, NULL), 1);
  } else if(!strcasecmp(str, "/back")) {
    return back_irc_conn(vc, ircc);
  } else if(!strncasecmp(str, "/topic ", 7) && strlen(str) > 7 && chan) {
    ln = str + 7;
    return topic_irc_conn(vc, make_list(vc, 3, ircc, chan, make_raw_string(vc, ln)), 3);
  } else if(!strcasecmp(str, "/topic") && chan) {
    return topic_irc_conn(vc, make_list(vc, 2, ircc, chan), 2);
  } else if(!strncasecmp(str, "/quit ", 6) && strlen(str) > 6) {    
    ln = str + 6;
    return quit_irc_conn(vc, ircc, make_raw_string(vc, ln));
  } else if(!strcasecmp(str, "/quit")) {
    return quit_irc_conn(vc, ircc, make_raw_string(vc, "client quit"));
  } else if(chan) {
    if(STR(chan)[0] == '#')
      fake_event(vc, IRCC(ircc), PUBLIC, STR(chan), NULL,
		 IRCC(ircc)->nick->buff, NULL, str);
    else
      fake_event(vc, IRCC(ircc), MSG, NULL, STR(chan), IRCC(ircc)->nick->buff, NULL, str);
    return privmsg_irc_conn(vc, ircc, chan, s);
  }
  return vc->false;
}

///////////////////////////////////////////////////////////////////////////
// Utility functions (idle, away)

void irc_set_away(IRC_CONN ircc) {
  time_t clock;

  ircc->away = time(&clock);
  irc_status_push(ircc, STATUS_AWAY);
}

VCSI_OBJECT away_irc_connq(VCSI_CONTEXT vc, VCSI_OBJECT x) {

  if(!same_type_user_def(vc, x, irc_conn_type))
    return error(vc, "objected passed to irc-away? is of the wrong type", x);
  
  if(IRCC(x)->away == 0) 
    return vc->false;

  return vc->true;
}

VCSI_OBJECT time_away_irc_conn(VCSI_CONTEXT vc, VCSI_OBJECT x) {

  long i = 0;
  time_t clock;

  if(!same_type_user_def(vc, x, irc_conn_type))
    return error(vc, "objected passed to irc-time-away is of the wrong type", x);

  if(IRCC(x)->away != 0)
    i = time(&clock) - IRCC(x)->away;
  
  return make_long(vc, i);
}


void irc_reset_idle(IRC_CONN ircc) {

  time_t clock;

  ircc->idle = time(&clock);
  irc_status_push(ircc, STATUS_NOT_IDLE);
}

VCSI_OBJECT time_idle_irc_conn(VCSI_CONTEXT vc, VCSI_OBJECT x) {

  long i = 0;
  time_t clock;

  if(!same_type_user_def(vc, x, irc_conn_type))
    return error(vc, "objected passed to irc-time-idle is of the wrong type", x);
  
  i = time(&clock) - IRCC(x)->idle;
    
  return make_long(vc, i);
}

///////////////////////////////////////////////////////////////////////////
// IRC Test Functions 
VCSI_OBJECT irc_connq(VCSI_CONTEXT vc, 
		      VCSI_OBJECT x) {

  if(!same_type_user_def(vc, x, irc_conn_type))
    return vc->false;
  return vc->true;
}

VCSI_OBJECT connected_irc_connq(VCSI_CONTEXT vc, 
				VCSI_OBJECT x) {

  if(!same_type_user_def(vc, x, irc_conn_type))
    return error(vc, "object passed to irc-connected? is of the wrong type", x);

  if(IRC_STATE_CHECK(IRCC(x), IRC_STATE_CONNECTED))
    return vc->true;
  return vc->false;
}

///////////////////////////////////////////////////////////////////////////
// Module required functions 

void print_irc_conn(VCSI_CONTEXT vc,
		    INFOSTD_DYN_STR res, 
		    VCSI_OBJECT obj) {

  infostd_dyn_str_printf(vc->res_extra, "#[irc %s:%d \"%s\" (%s)]",
			 IRCC(obj)->server->buff,
			 IRCC(obj)->port,
			 IRCC(obj)->nick->buff,
			 (IRC_STATE_CHECK(IRCC(obj), IRC_STATE_CONNECTED)) ? 
			 "connected" : "disconnected");
  
  infostd_dyn_str_strcat(res, vc->res_extra->buff);
}

void mark_irc_conn(VCSI_CONTEXT vc,
		   VCSI_OBJECT obj, 
		   int flag) {

  /* mark all the handlers */
  gc_mark(vc, IRCC(obj)->on_system, flag);
  gc_mark(vc, IRCC(obj)->on_notice, flag);
  gc_mark(vc, IRCC(obj)->on_error, flag);
  gc_mark(vc, IRCC(obj)->on_public, flag);
  gc_mark(vc, IRCC(obj)->on_msg, flag);
  gc_mark(vc, IRCC(obj)->on_action, flag);
  gc_mark(vc, IRCC(obj)->on_ctcp, flag);

  gc_mark(vc, IRCC(obj)->on_ctcp_ping, flag);
  gc_mark(vc, IRCC(obj)->on_ctcp_version, flag);

  gc_mark(vc, IRCC(obj)->on_connect, flag);
  gc_mark(vc, IRCC(obj)->on_join, flag);
  gc_mark(vc, IRCC(obj)->on_part, flag);
  gc_mark(vc, IRCC(obj)->on_quit, flag);
  gc_mark(vc, IRCC(obj)->on_disconnect, flag);

  gc_mark(vc, IRCC(obj)->on_nick_change, flag);
  gc_mark(vc, IRCC(obj)->on_nick_taken, flag);
  gc_mark(vc, IRCC(obj)->on_kick, flag);

  gc_mark(vc, IRCC(obj)->on_names, flag);
  gc_mark(vc, IRCC(obj)->on_topic, flag);
  gc_mark(vc, IRCC(obj)->on_topic_who, flag);

  gc_mark(vc, IRCC(obj)->on_is_away, flag);
  gc_mark(vc, IRCC(obj)->on_away, flag);
  gc_mark(vc, IRCC(obj)->on_back, flag);

  gc_mark(vc, IRCC(obj)->on_ison, flag);
}

void free_irc_conn(VCSI_CONTEXT vc,
		   VCSI_OBJECT obj) {

  if(IRC_STATE_CHECK(IRCC(obj), IRC_STATE_CONNECTED))
    close(IRCC(obj)->skt);

  FREE(IRCC(obj));
}

VCSI_OBJECT eval_irc_conn(VCSI_CONTEXT vc,
			  VCSI_OBJECT args, 
			  int length) {

  VCSI_OBJECT method, arg, arg1, arg2;
  char *mstr, *arg1s;
  VCSI_OBJECT irc;
  int mlen, alen;
  IRC_CONN ircc;

  method = arg1 = arg2 = NULL;

  if(length < 1 || length > 4)
    error(vc, "invalid number of arguments to irc-conn", args);

  // Make sure we are evaluating ourselves
  irc = car(vc, args);
  if(!same_type_user_def(vc, irc, irc_conn_type))
    return error(vc,"object passed to irc-conn is of the wrong type", irc);
  
  // No Method return the object
  if(length < 2)
    return irc;

  ircc = IRCC(irc);

  // Setup a new list of the arguments with irc-obj but not method
  arg = cons(vc, irc, cddr(vc, args));
  alen = get_length(arg);

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
  if(!strcasecmp(mstr, "server")) {
    if(arg1)
      return server_irc_conn(vc, irc, arg1);
    else
      return make_string(vc, ircc->server->buff);
  } else if(!strcasecmp(mstr, "port")) {
    if(arg1)
      return port_irc_conn(vc, irc, arg1);
    else
      return make_long(vc, ircc->port);
  } else if(!strcasecmp(mstr, "nick")) {
    if(arg1)
      return nick_irc_conn(vc, irc, arg1);
    else
      return make_string(vc, ircc->nick->buff);
  } else if(!strcasecmp(mstr, "name")) {
    if(arg1)
      return name_irc_conn(vc, irc, arg1);
    else
      return make_string(vc, ircc->name->buff);
  } else if(!strcasecmp(mstr, "channel")) {
      return make_string(vc, ircc->current_chan->buff);
  } else if(!strcasecmp(mstr, "handler")) {
    return set_handler_irc_conn(vc, irc, arg1, arg2);
  } else if(!strcasecmp(mstr, "event") || !strcasecmp(mstr, "handle-one-event")) {
    return handle_one_event_irc_conn(vc, irc);
  } else if(!strcasecmp(mstr, "event-ready?")) {
    return event_ready_irc_conn(vc, irc);
  } else if(!strcasecmp(mstr, "connect")) {
    return connect_irc_conn(vc, irc);
  } else if(!strcasecmp(mstr, "disconnect")) {
    return disconnect_irc_conn(vc, irc);
  } else if(!strcasecmp(mstr, "connected?")) {
    return connected_irc_connq(vc, irc);
  } else if(!strcasecmp(mstr, "irc?")) {
    return vc->true;
  } else if(!strcasecmp(mstr, "raw")) {
    return raw_irc_conn(vc, irc, arg1);
  } else if(!strcasecmp(mstr, "quit")) {
    if(arg1)
      return quit_irc_conn(vc, irc, arg1);
    else
      return quit_irc_conn(vc, irc, make_string(vc, "client quit"));
  } else if(!strcasecmp(mstr, "join")) {
    return join_irc_conn(vc, irc, arg1);
  } else if(!strcasecmp(mstr, "part")) {
    return part_irc_conn(vc, irc, arg1);
  } else if(!strcasecmp(mstr, "notice")) {
    return notice_irc_conn(vc, irc, arg1, arg2);
  } else if(!strcasecmp(mstr, "msg") || !strcasecmp(mstr, "privmsg")) {
    return privmsg_irc_conn(vc, irc, arg1, arg2);
  } else if(!strcasecmp(mstr, "me")) {
    return me_irc_conn(vc, irc, arg1, arg2);
  } else if(!strcasecmp(mstr, "ctcp")) {
    return ctcp_irc_conn(vc, irc, arg1, arg2);
  } else if(!strcasecmp(mstr, "topic")) {
    return topic_irc_conn(vc, arg, alen);
  } else if(!strcasecmp(mstr, "names")) {
    return names_irc_conn(vc, arg, alen);
  } else if(!strcasecmp(mstr, "kick")) {
    return kick_irc_conn(vc, arg, alen);
  } else if(!strcasecmp(mstr, "away")) {
    return away_irc_conn(vc, arg, alen);
  } else if(!strcasecmp(mstr, "away-msg") || !strcasecmp(mstr, "away-message")) {
    return make_string(vc, ircc->away_msg->buff);
  } else if(!strcasecmp(mstr, "back")) {
    return back_irc_conn(vc, irc);
  } else if(!strcasecmp(mstr, "ison")) {
    return ison_irc_conn(vc, irc, arg1);
  } else if(!strcasecmp(mstr, "away?")) {
    return away_irc_connq(vc, irc);
  } else if(!strcasecmp(mstr, "away-time")) {
    return time_away_irc_conn(vc, irc);
  } else if(!strcasecmp(mstr, "idle-time")) {
    return time_idle_irc_conn(vc, irc);
  } else if(!strcasecmp(mstr, "squeue") || !strcasecmp(mstr, "status-queue")) {
    if(arg1) {
      if(TYPEP(arg1, BOOLEAN)) {
	if(BOOL(arg1)) {
	  enable_status_queue_irc_conn(vc, irc);
	} else {
	  disable_status_queue_irc_conn(vc, irc);
	}
	return (IRC_STATE_CHECK(ircc, IRC_STATE_SQUEUE)) ? vc->true : vc->false;
      } else if(TYPEP(arg1, STRING) || TYPEP(arg1, SYMBOL)) {
	arg1s = (TYPEP(arg1, SYMBOL)) ? SNAME(arg1) : STR(arg1);
	if(!strcasecmp(arg1s, "pop")) {
	  return pop_status_queue_irc_conn(vc, irc);
	} else if(!strcasecmp(arg1s, "enabled?")) {
	  return (IRC_STATE_CHECK(ircc, IRC_STATE_SQUEUE)) ? vc->true : vc->false;
	} else if(!strcasecmp(arg1s, "enable")) {
	  return enable_status_queue_irc_conn(vc, irc);
	} else if(!strcasecmp(arg1s, "disable")) {
	  return disable_status_queue_irc_conn(vc, irc);
	} else {
	  return error(vc, "unknown status queue method to irc-conn", arg1);
	}
      }
    } else
      return error(vc, "no status queue method to irc-conn", arg1);
  } else if(!strcasecmp(mstr, "ping-reply")) {
    return ping_reply_irc_conn(vc, irc, arg1, arg2);
  } else if(!strcasecmp(mstr, "version-reply")) {
    return version_reply_irc_conn(vc, irc, arg1, arg2);
  } else if(!strcasecmp(mstr, "parse")) {
    return command_parse_irc_conn(vc, arg, alen);
  }
  
  return vc->false;
}

void module_init(VCSI_CONTEXT vc) {
  irc_conn_type = make_user_def_type(vc, "harangue-irc", print_irc_conn, 
				     mark_irc_conn, free_irc_conn);

  set_int_proc(vc, "make-irc", PROC0, init_irc_conn);

  set_int_proc(vc, "irc?", PROC1, irc_connq);

  set_int_proc(vc, "irc-server", PROC2, server_irc_conn);
  set_int_proc(vc, "irc-port", PROC2, port_irc_conn);
  set_int_proc(vc, "irc-nick", PROC2, nick_irc_conn);
  set_int_proc(vc, "irc-name", PROC2, name_irc_conn);

  set_int_proc(vc, "irc-handler-set!", PROC3, set_handler_irc_conn);
  set_int_proc(vc, "irc-handle-one-event", PROC1, handle_one_event_irc_conn);
  set_int_proc(vc, "irc-event-ready?", PROC1, event_ready_irc_conn);
  set_int_proc(vc, "irc-connect", PROC1, connect_irc_conn);
  set_int_proc(vc, "irc-disconnect", PROC1, disconnect_irc_conn);
  set_int_proc(vc, "irc-connected?", PROC1, connected_irc_connq);

  // Does Status Queue need to be exposed to scheme? how?
  set_int_proc(vc, "irc-status-queue-enable", PROC1, enable_status_queue_irc_conn);
  set_int_proc(vc, "irc-status-queue-disable", PROC1, disable_status_queue_irc_conn);
  set_int_proc(vc, "irc-status-queue-pop", PROC1, pop_status_queue_irc_conn);

  set_int_proc(vc, "irc-raw", PROC2, raw_irc_conn);
  set_int_proc(vc, "irc-quit", PROC2, quit_irc_conn);
  set_int_proc(vc, "irc-join", PROC2, join_irc_conn);
  set_int_proc(vc, "irc-part", PROC2, part_irc_conn);
  set_int_proc(vc, "irc-notice", PROC3, notice_irc_conn);
  set_int_proc(vc, "irc-privmsg", PROC3, privmsg_irc_conn);
  set_int_proc(vc, "irc-me", PROC3, me_irc_conn);
  set_int_proc(vc, "irc-ctcp", PROC3, ctcp_irc_conn);

  set_int_proc(vc, "irc-topic", PROCOPT, topic_irc_conn);
  set_int_proc(vc, "irc-names", PROCOPT, names_irc_conn);
  set_int_proc(vc, "irc-kick", PROCOPT, kick_irc_conn);
  set_int_proc(vc, "irc-away", PROCOPT, away_irc_conn);

  set_int_proc(vc, "irc-away-message", PROC1, away_msg_irc_conn);
  set_int_proc(vc, "irc-back", PROC1, back_irc_conn);
  set_int_proc(vc, "irc-ison", PROC2, ison_irc_conn);

  set_int_proc(vc, "irc-away?", PROC1, away_irc_connq);
  set_int_proc(vc, "irc-time-away", PROC1, time_away_irc_conn);
  set_int_proc(vc, "irc-time-idle", PROC1, time_idle_irc_conn);

  set_int_proc(vc, "irc-command-parse", PROCOPT, command_parse_irc_conn);

  set_int_proc(vc, "irc-ping-reply", PROC3, ping_reply_irc_conn);
  set_int_proc(vc, "irc-version-reply", PROC3, version_reply_irc_conn);
  
  /* To Do:
     oper?, mode, list, invite, info?, who, whois, whowas?, userhost
  */
}
