# --------------------------------
# The f8 (fate) scripting language
# --------------------------------
#
# Example: functional programming
#
# (c) www.carminecella.com
#

source "stdlib.tcl"

# tail-recursive functions
proc (mylen l) {
	proc (mylen-runner i l) {
		if (eq l ()) i	else (mylen-runner (+ i 1) (cdr l))
	}
	mylen-runner 0 l
}
puts "length of a long list = " (mylen (tolist (noise 10000))) "\n"

# non tail-recursive functions - it may crash if applied to very long lists
proc (mydot a b) {
	if (or (eq a ()) (eq b ())) 0 else {
		+ (* (car a) (car b)) (mydot (cdr a) (cdr b))
	}
}
puts "dot product of two (short) lists = " (mydot '(1 2 3) '(4 5 6)) "\n\n"

# flip operator
proc (myzeros)(flip dup 0)
proc (myones)(flip dup 1)
puts "sequences of 0s and 1s =\n" 
puts ((myzeros) 5) " " ((myones) 5) "\n\n"

# foldl 
proc (mysum l)(foldl + 0 l)
proc (myprod l)(foldl * 1 l)
proc (mymean l)(/ (foldl + 0 l) (mylen l))
puts "sum of a list  = " (mysum '(1 2 3 4)) "\n"
puts "prod of a list  = " (myprod '(1 2 3 4)) "\n"
puts "mean of a list  = " (mymean '(1 2 3 4)) "\n\n"

# compare 
proc (mymin l) ((compare <) l)
proc (mymax l) ((compare >) l)
puts "min of a list  = " (mymin '(1 2 3 4)) "\n"
puts "max of a list  = " (mymax '(1 2 3 4)) "\n\n"

# eof


