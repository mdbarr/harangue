;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; 
;; User redefines and functions
;;

;; Set our Nick
(harangue 'nick "gary")

;; Proxy to Address (not our own)
(define proxy-buddy "fm")

;; Custom CTCP Handler (gamble stats, proxy)
(define (irc-custom-ctcp-handler channel target source hostmask text)
  (cond ((=~ text "/GAMBLE UPDATE_CASH ([0-9]+)/")
	 (harangue 'status1 (format "$~a" (comma-format $1))))
	((=~ text "/PROXY_MSG (\\S+) (.+)/")
	 (harangue-display-private $1 "proxy-msg" $2))
	(else
	 (harangue-display-ctcp source hostmask text))))

(harangue 'handler 'ctcp irc-custom-ctcp-handler)

;; Custom MSG Handler (proxy)
(define (irc-custom-msg-handler channel target source hostmask text)
  (if (not (string-ci=? source proxy-buddy))
      (harangue 'ctcp proxy-buddy (format "PROXY_MSG ~a ~a" source text)))
  (harangue-display-private source hostmask text))

(harangue 'handler 'msg irc-custom-msg-handler)

;; Custom CONNECT Handler
(define (irc-connect-handler channel target source hostmask text)
  (harangue 'ctcp "Sasquatch" "GAMBLE_UPDATE_OPTIN")
  (harangue 'join "#sass"))

(harangue 'handler 'connect irc-connect-handler)

(define (irc-love who)
  (harangue 'msg who " ___   _     _____     _______  __   _____  _   _   _   _")
  (harangue 'msg who "|_ _| | |   / _ \\ \\   / / ____| \\ \\ / / _ \\| | | | | | | |")
  (harangue 'msg who " | |  | |  | | | \\ \\ / /|  _|    \\ V / | | | | | | | | | |")
  (harangue 'msg who " | |  | |__| |_| |\\ V / | |___    | || |_| | |_| | |_| |_|")
  (harangue 'msg who "|___| |_____\\___/  \\_/  |_____|   |_| \\___/ \\___/  (_) (_)")
)

;;
#t