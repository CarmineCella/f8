# --------------------------------
# The f8 (fate) scripting language
# --------------------------------
#
# Evlis recursion tests
#
# (c) www.carminecella.com
#
source "stdlib.tcl"

puts "--- evlis recursion ---\n\n"

# infinite recursion - this never stops
proc (rec x){
	puts x "\n" 
	rec (+ 1 x)
}

# evlis tail recursion - this crashes 
proc (trec x) {
	puts x "\n" 
	+ 1 (trec (+ 1 x))
}

puts "enter:\n"
puts "\t 1 to start evlis recursion (it will crash)\n"
puts "\t 2 to start infinite recursion\n\n"

set a (gets)

if (eq a '(1)) {
	trec 1
} else {
	rec 1
}

# eof

