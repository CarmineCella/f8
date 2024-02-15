# --------------------------------
# The f8 (fate) scripting language
# --------------------------------
#
# Dynamic scope tests
#
# (c) www.carminecella.com
#

source "stdlib.tcl"

set b 5

proc (static_foo){
	+ b 5
}

proc (static_bar) {
	set b 2
	static_foo
}

dynamic (dynamic_foo) {
	+ b 5
}

proc (dynamic_bar) {
	set b 2
	dynamic_foo
}
 
 
 puts  "--- static vs dynamic scope ---\n\n"

test '(static_foo) '10
test '(static_bar) '10

test (dynamic_foo) '10
test (dynamic_bar) '7

puts "\nALL TESTS PASSED\n\n"