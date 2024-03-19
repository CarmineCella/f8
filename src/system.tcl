# --------------------------------
# The f8 (fate) scripting language
# --------------------------------
#
# System library
#
# (c) www.carminecella.com
#

proc (test x y) {
	if (eq (eval x) y) (puts x  " passed\n") else (throw (tostr x " *** FAILED ***"))
}
proc (fexists fname) {
	eq (car (filestat fname)) 1
}
proc (readlines stream) {	
	set lines ()
	proc (readlines-runner stream lines) {
		set line (readline stream)
		if (not (eq line "")) (lappend lines line)
		if (not (isgood stream)) lines else {
			readlines-runner stream lines
		}
	}
	readlines-runner stream lines
}
proc (udpmonitor addr port) {
	set r (udprecv addr port)
	if (eq r "disconnect") 1 else {
		puts r nl
		udpmonitor addr port
	}
}

# eof

