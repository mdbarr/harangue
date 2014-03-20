;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Harangue internal definition and handler script
;;

;; Set configuration variables
(harangue 'server "irc.snoogins.com")
;;(harangue 'port 6667)

(harangue 'nick "testy")
(harangue 'name "harangue-client")

;;(harangue 'timestamp #t) ;; Default is On
;;(harangue 'hostmask #f) ;; Default is Off

;; Create simple handlers
(define (irc-disconnect-handler channel target source hostmask text)
  (harangue-display-system (format "Disconnected from ~a" (harangue 'server))))

(define (irc-system-handler channel target source hostmask text)
  (harangue-display-system text))

(define (irc-public-handler channel target source hostmask text)
  (harangue-display-public source channel text))

(define (irc-msg-handler channel target source hostmask text)
  (harangue-display-private source hostmask text))

(define (irc-action-handler channel target source hostmask text)
  (harangue-display-action source channel text))

(define (irc-notice-handler channel target source hostmask text)
  (harangue-display-notice source hostmask text))

(define (irc-ctcp-handler channel target source hostmask text)
  (harangue-display-ctcp source hostmask text))

(define (irc-topic-handler channel target source hostmask topic)
  (if (not (null? source))
      (harangue-display-channel-message 
       "topic" channel (format "changed by ~a: ~a" 
			       (harangue-colorize-nick source) topic))
      (harangue-display-system 
       (format "Topic for ~a: ~a" (harangue-colorize-nick channel) topic))))

(define (irc-topic-who-handler channel target source hostmask text)
  (if (=~ text "/(.*)!.* ([0-9]+)/")
      (harangue-display-system (format "Topic set by ~a on ~a" (harangue-colorize-nick $1)
				       (localtime (string->number $2))))))

(define (irc-kick-handler channel target source hostmask text)
  (harangue-display-channel-message "kick" channel
				    (format "~a by ~a (~a)" (harangue-colorize-nick target)
					    (harangue-colorize-nick source) text))
  (if (eq? target (harangue 'nick)) (harangue 'join channel)))

(define (irc-join-handler channel target source hostmask text)
  (if (eq? source (harangue 'nick))
      (window-title-set! (format "~a - harangue" channel)))
  (harangue-display-channel-message "join" channel 
				    (format "~a!~a" (harangue-colorize-nick source) hostmask)))

(define (irc-part-handler channel target source hostmask text)
  (harangue-display-channel-message "part" channel 
				    (format "~a!~a" (harangue-colorize-nick source) hostmask)))

(define (irc-quit-handler channel target source hostmask text)
  (harangue-display-channel-message "signoff" "system"
				    (format "~a (~a)" (harangue-colorize-nick source) text)))

(define (irc-nick-taken-handler channel target source hostmask text)
  (harangue-display-system (format "~a: ~a" target text))
  (harangue 'nick (format "~a_" target)))

(define (irc-nick-change-handler channel target source hostmask text)
  (harangue-display-nick-change source text))

(define (irc-ping-handler channel target source hostmask text)
  (harangue 'ping-reply source text))

(define (irc-version-handler channel target source hostmask text)
  (harangue 'version-reply source harangue-version))

(define (irc-away-handler channel target source hostmask text)
  (harangue-command-parse (format "/me is now gone 4[~a]" 
				  (harangue 'away-message)))
  (harangue-display-system "You are now marked as being away."))

(define (irc-back-handler channel target source hostmask text)
  (harangue-command-parse (format "/me is now back 3[Gone ~a - ~a -]" 
				  (harangue-string-time (harangue 'time-away))
				  (harangue 'away-message)))
  (harangue-display-system "Welcome back!"))

(define (irc-is-away-handler channel target source hostmask text)
  (harangue-display-system (format "~a is away: ~a" target text)))

(define (irc-names-handler channel target source hostmask text)
  (harangue-display-channel-names channel (sort (string-tokenize " " text))))

(define (irc-ison-handler channel target source hostmask text)
  (if text
        (harangue-display-system (format "~a is online" (harangue-colorize-nick text)))))

(define (irc-whats-going-on a b c d e)
  (harangue-display-system (format "-C:~a T:~a S:~a H:~a T:~a-" a b c d e)))

;; Set the handlers
(harangue 'handler 'disconnect irc-disconnect-handler)
(harangue 'handler 'system irc-system-handler)
(harangue 'handler 'public irc-public-handler)
(harangue 'handler 'msg irc-msg-handler)
(harangue 'handler 'action irc-action-handler)
(harangue 'handler 'notice irc-notice-handler)
(harangue 'handler 'ctcp irc-ctcp-handler)
(harangue 'handler 'kick irc-kick-handler)
(harangue 'handler 'topic irc-topic-handler)
(harangue 'handler 'topic-who irc-topic-who-handler)
(harangue 'handler 'join irc-join-handler)
(harangue 'handler 'part irc-part-handler)
(harangue 'handler 'quit irc-quit-handler)
(harangue 'handler 'nick-taken irc-nick-taken-handler)
(harangue 'handler 'nick-change irc-nick-change-handler)
(harangue 'handler 'ping irc-ping-handler)
(harangue 'handler 'version irc-version-handler)
(harangue 'handler 'away irc-away-handler)
(harangue 'handler 'back irc-back-handler)
(harangue 'handler 'is-away irc-is-away-handler)
(harangue 'handler 'names irc-names-handler)
(harangue 'handler 'ison irc-ison-handler)

;; Load Default Speller
(harangue-speller-load '())

;; Done
#t
