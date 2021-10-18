Intended purpose : 
  An irc client for Mystic BBS

How to compile : 
  Because this project uses ncurses, something along the lines of the following :
  gcc -o test test.c -lncurses

ToDo List : 
  * Clear buffer of previous content
  * Continue to add functionality commands (NICK/WHOIS/Private Messaging)
  * Fix the display, it doesn't work correctly
  * Add logic to prevent > 80 characters in a message
  * Display has ascii codes, not characters
