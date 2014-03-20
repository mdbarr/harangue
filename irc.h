/********************************************************************************
 *
 * VCSI IRC Module
 *
 ********************************************************************************/

#define IRC_DEFAULT_SERVER "chat.freenode.net"
#define IRC_DEFAULT_PORT   6667
#define IRC_DEFAULT_NICK   "nobody"
#define IRC_DEFAULT_NAME   "No Name"

#define IRC_STATE_NONE             0
#define IRC_STATE_CONNECTED  (1 << 0)
#define IRC_STATE_SQUEUE     (1 << 1)

#define IRC_STATE_SET(x,flag) (x->state |= flag)
#define IRC_STATE_UNSET(x,flag) (x->state &= ~flag)
#define IRC_STATE_CHECK(x,flag) (x->state & flag)

// If set causes the module to error out when 'no connected'
#undef IRC_NOT_CONNECTED_ERROR

#define IRCBUF 1024
#define IRC_DYNSTR_DEFAULT 128
#define IRC_DYNSTR_TEXT_DEFAULT 1024


// IRC Connection Structure
typedef struct str_irc_conn {

  /* The Actual Socket */
  int skt;

  /* The State of the Connection */
  int state;

  /* Server and Port */
  INFOSTD_DYN_STR server;
  long port;

  /* Nick and IRC Name */
  INFOSTD_DYN_STR nick;
  INFOSTD_DYN_STR name;

  ///////////////////////////////////////////////////////
  /* Idle and Away counters */
  unsigned long idle;

  unsigned long away;
  INFOSTD_DYN_STR away_msg;

  /* Status changes Queue */
  INFOSTD_CQUEUE squeue;

  /* Internal memory of Channels and Nicks */
  INFOSTD_DYN_STR last_msg;

  INFOSTD_DYN_STR current_chan;

  INFOSTD_DYN_STRS channel_list;
  INFOSTD_DYN_STRS nick_list;

  ///////////////////////////////////////////////////////

  /* Callbacks */

  /* Messages */
  VCSI_OBJECT on_system; /* system notification */
  VCSI_OBJECT on_notice; /* general notices */
  VCSI_OBJECT on_error; /* error notices */
  VCSI_OBJECT on_public; /* public msg */
  VCSI_OBJECT on_msg; /* private msg */
  VCSI_OBJECT on_action; /* action */
  VCSI_OBJECT on_ctcp; /* generic ctcp */

  /* CTCPs */
  VCSI_OBJECT on_ctcp_ping; /* ping */
  VCSI_OBJECT on_ctcp_version; /* version ping */

  /* Server / Channel stuff */
  VCSI_OBJECT on_connect; /* when connected to the server */
  VCSI_OBJECT on_join; /* when someone joins the channel */
  VCSI_OBJECT on_part; /* when someone leaves the channel */
  VCSI_OBJECT on_quit; /* when someone quits */
  VCSI_OBJECT on_disconnect; /* when we are disconnected */

  /* Other Stuff */
  VCSI_OBJECT on_nick_change; /* someone changed their nick */
  VCSI_OBJECT on_nick_taken; /* our nick is taken */
  VCSI_OBJECT on_kick; /* when we are kicked */

  /* Command Responses */
  VCSI_OBJECT on_names; /* results of a names command */
  VCSI_OBJECT on_topic; /* when the topic changes */
  VCSI_OBJECT on_topic_who; /* when the topic changes */

  VCSI_OBJECT on_is_away; /* a user is away */
  VCSI_OBJECT on_away; /* when we go away */
  VCSI_OBJECT on_back; /* when we are back */

  VCSI_OBJECT on_ison; /* results of /ison */
  
  /* Buffers with extra space (IRC messages are limited to 512 chars)*/
  char send_buf[IRCBUF];
  char recv_buf[IRCBUF];
} *IRC_CONN;
#define SIZE_IRC_CONN (sizeof(struct str_irc_conn))

typedef enum irc_event_type {
  NOTICE, ERROR, PING, PONG, PUBLIC, MSG, ACTION, CTCP, CTCP_PING, 
  CTCP_VERSION, CONNECT, JOIN, PART, QUIT, DISCONNECT, NICK_CHANGE, 
  NICK_TAKEN, KICK, NAMES, TOPIC, TOPIC_WHO, IS_AWAY, AWAY, BACK, 
  ISON, SYSTEM
} IRC_EVENT_TYPE;

/* IRC Event */
typedef struct str_irc_event {
  enum irc_event_type type;
  INFOSTD_DYN_STR channel;
  INFOSTD_DYN_STR target;
  INFOSTD_DYN_STR nick;
  INFOSTD_DYN_STR host;
  INFOSTD_DYN_STR text;
} *IRC_EVENT;
#define SIZE_IRC_EVENT (sizeof(struct str_irc_event))

/* status changes */
typedef enum irc_status_type {
  STATUS_NONE, STATUS_IDLE, STATUS_NOT_IDLE, STATUS_AWAY, STATUS_BACK, 
  STATUS_QUIT, STATUS_NICK_CHANGED, STATUS_JOINED, STATUS_PARTED,
} IRC_STATUS;

/* user defined type stuff */
VCSI_USER_DEF irc_conn_type;
#define IRCC(x) ((IRC_CONN)(*x).data.userdef.ud_value)

IRC_EVENT irc_event_init(void);
void irc_event_free(IRC_EVENT irce);

IRC_EVENT parse_event(char* etext);

VCSI_OBJECT handle_ping_event(VCSI_CONTEXT vc, 
			      IRC_CONN ircc, 
			      IRC_EVENT irce);

int irc_raw_ping(IRC_CONN ircc);

VCSI_OBJECT handle_event(VCSI_CONTEXT vc, 
			 IRC_CONN ircc, 
			 IRC_EVENT irce);

void fake_event(VCSI_CONTEXT vc,
		IRC_CONN ircc,
		IRC_EVENT_TYPE type,
		char *channel,
		char *target,
		char *source,
		char *hostmask,
		char *text);

IRC_STATUS irc_status_pop(IRC_CONN ircc);

void irc_status_push(IRC_CONN ircc, IRC_STATUS s);

IRC_CONN irc_connection_init(void);

void irc_connection_free(IRC_CONN ircc);

VCSI_OBJECT error_irc_conn(VCSI_CONTEXT vc,
			   char *err,
			   VCSI_OBJECT e);

VCSI_OBJECT init_irc_conn(VCSI_CONTEXT vc);
		
VCSI_OBJECT connect_irc_conn(VCSI_CONTEXT vc, 
			     VCSI_OBJECT x);

VCSI_OBJECT disconnect_irc_conn(VCSI_CONTEXT vc, 
				VCSI_OBJECT x);

VCSI_OBJECT enable_status_queue_irc_conn(VCSI_CONTEXT vc, 
					 VCSI_OBJECT x);

VCSI_OBJECT disable_status_queue_irc_conn(VCSI_CONTEXT vc, 
					  VCSI_OBJECT x);

VCSI_OBJECT pop_status_queue_irc_conn(VCSI_CONTEXT vc,
                                      VCSI_OBJECT x);

VCSI_OBJECT raw_irc_conn(VCSI_CONTEXT vc, 
			 VCSI_OBJECT x, 
			 VCSI_OBJECT y);

VCSI_OBJECT quit_irc_conn(VCSI_CONTEXT vc, 
			  VCSI_OBJECT x, 
			  VCSI_OBJECT y);

VCSI_OBJECT join_irc_conn(VCSI_CONTEXT vc, 
			  VCSI_OBJECT x, 
			  VCSI_OBJECT y);

VCSI_OBJECT part_irc_conn(VCSI_CONTEXT vc, 
			  VCSI_OBJECT x, 
			  VCSI_OBJECT y);

VCSI_OBJECT nick_irc_conn(VCSI_CONTEXT vc,
			  VCSI_OBJECT x,
			  VCSI_OBJECT y);

VCSI_OBJECT name_irc_conn(VCSI_CONTEXT vc, 
			  VCSI_OBJECT x, 
			  VCSI_OBJECT y);

VCSI_OBJECT server_irc_conn(VCSI_CONTEXT vc, 
			    VCSI_OBJECT x, 
			    VCSI_OBJECT y);

VCSI_OBJECT port_irc_conn(VCSI_CONTEXT vc, 
			    VCSI_OBJECT x, 
			    VCSI_OBJECT y);

VCSI_OBJECT notice_irc_conn(VCSI_CONTEXT vc, 
			    VCSI_OBJECT x, 
			    VCSI_OBJECT y, 
			    VCSI_OBJECT z);

VCSI_OBJECT privmsg_irc_conn(VCSI_CONTEXT vc, 
			     VCSI_OBJECT x, 
			     VCSI_OBJECT y, 
			     VCSI_OBJECT z);

VCSI_OBJECT me_irc_conn(VCSI_CONTEXT vc, 
			VCSI_OBJECT x, 
			VCSI_OBJECT y, 
			VCSI_OBJECT z);

VCSI_OBJECT ctcp_irc_conn(VCSI_CONTEXT vc, 
			  VCSI_OBJECT x, 
			  VCSI_OBJECT y, 
			  VCSI_OBJECT z);

VCSI_OBJECT topic_irc_conn(VCSI_CONTEXT vc, 
			   VCSI_OBJECT args, 
			   int length);

VCSI_OBJECT names_irc_conn(VCSI_CONTEXT vc,
			   VCSI_OBJECT args,
			   int length);

VCSI_OBJECT kick_irc_conn(VCSI_CONTEXT vc, 
			  VCSI_OBJECT args, 
			  int length);

VCSI_OBJECT away_irc_conn(VCSI_CONTEXT vc,
			  VCSI_OBJECT args,
			  int length);

VCSI_OBJECT away_msg_irc_conn(VCSI_CONTEXT vc,
                              VCSI_OBJECT x);

VCSI_OBJECT back_irc_conn(VCSI_CONTEXT vc, 
			  VCSI_OBJECT x);

VCSI_OBJECT ison_irc_conn(VCSI_CONTEXT vc, 
			  VCSI_OBJECT x, 
			  VCSI_OBJECT y);

VCSI_OBJECT ping_reply_irc_conn(VCSI_CONTEXT vc, 
				VCSI_OBJECT x, 
				VCSI_OBJECT y,
				VCSI_OBJECT z);

VCSI_OBJECT version_reply_irc_conn(VCSI_CONTEXT vc, 
				   VCSI_OBJECT x, 
				   VCSI_OBJECT y,
				   VCSI_OBJECT z);

VCSI_OBJECT set_handler_irc_conn(VCSI_CONTEXT vc, 
				 VCSI_OBJECT x, 
				 VCSI_OBJECT y, 
				 VCSI_OBJECT z);

VCSI_OBJECT handle_one_event_irc_conn(VCSI_CONTEXT vc, 
				      VCSI_OBJECT x);

VCSI_OBJECT event_ready_irc_conn(VCSI_CONTEXT vc,
				 VCSI_OBJECT x);

VCSI_OBJECT command_parse_irc_conn(VCSI_CONTEXT vc,
				   VCSI_OBJECT args,
				   int length);

void irc_set_away(IRC_CONN ircc);

void irc_reset_idle(IRC_CONN ircc);

VCSI_OBJECT irc_connq(VCSI_CONTEXT vc, 
		      VCSI_OBJECT x);

VCSI_OBJECT connected_irc_connq(VCSI_CONTEXT vc, 
				VCSI_OBJECT x);

void print_irc_conn(VCSI_CONTEXT vc,
		    INFOSTD_DYN_STR res, 
		    VCSI_OBJECT obj);

void mark_irc_conn(VCSI_CONTEXT vc,
		   VCSI_OBJECT obj, 
		   int flag);

void free_irc_conn(VCSI_CONTEXT vc,
		   VCSI_OBJECT obj);

VCSI_OBJECT eval_irc_conn(VCSI_CONTEXT vc,
			  VCSI_OBJECT args, 
			  int length);

void irc_module_init(VCSI_CONTEXT vc);
