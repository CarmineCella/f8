# --------------------------------
# The f8 (fate) scripting language
# --------------------------------
#
# Example: overview of some key features
#
# (c) www.carminecella.com
#
source "stdlib.tcl"

puts "** hallo f8!! **\n\n"

puts "** inrtospection **" "\n" "\n"
puts "the list of current variables is: " "\n"
puts (info 'vars) "\n" "\n"
puts "the list of current variables starting with letter l is: " "\n"
puts (info 'vars "l.*") "\n" "\n"
puts "there are several types, among which: " "\n"
puts '(a b c d) " = " (info 'typeof '(a b c d)) "\n"
puts 'a_symbol " = " (info 'typeof 'a_symbol) "\n"
puts "a simple string = " (info 'typeof "a simple string") "\n"
puts 4  " = " (info 'typeof 4) "\n"
puts "map = "(info 'typeof map) "\n" 
puts + " = " (info 'typeof +) "\n" "\n"

puts "** scopes are local to procs **" "\n" "\n"
puts "define proc alpha and proc beta inside it" "\n"
proc (alpha x) {
	proc (beta x) {
		puts x
	}
	puts "call beta from alpha: "
	beta 4
}
puts (alpha 5) "\n"
puts "does beta exist here? " (info 'exists 'beta) "\n" "\n"

puts "** dynamic scope **" "\n" "\n"
set b 5
puts "define dynamic foo" "\n" 
dynamic (foo) {
   set a (+ b 5)
}
puts "define static bar" "\n" 
proc (bar) {
   set b 2
   foo
}
puts "since foo is a dynamic procedure and bar is a static procedure:" "\n"
puts "foo: " foo "\n" "bar: " bar "\n"
puts "when you call it you get " (foo) "\n"
puts "buf if you call it through bar instead you get " (bar) "\n" "\n"

puts "** while loops with flow control **" "\n" "\n"
set b 10
puts "start from " b "\n"
if (> b 5) {
	set b (+ b 10)
	puts "this is " b " now... go higher!" "\n"
} else {
	puts "not executed :/" "\n"
}
puts "a loop..." "\n"
while (> b 0) {
	set b (- b 1)
	if (eq b 5) {
		puts "here we stop at " b "\n"
		break
	} 
	puts b "\n"
}
puts "\n"

puts "** recursion **" "\n" "\n"
proc (bad_fib x) {
    if (<= x 1) 1 else {
        + (bad_fib (- x 1)) (fib (- x 2))
    }
}
puts "fibonacci of 20 is " (bad_fib 20) "\n"
puts "\n"

puts "** higher functions with lambdas **" "\n" "\n"
puts "map " (map (\(x)(* x x)) '(1 2 3 4 5)) "\n"
puts "filter " (filter (\ (x) (> x 5)) '(5 2 11 -7 8 1)) "\n"
puts "unpack " (unpack + (list 4 5)) "\n" "\n"

puts "** closures **" "\n" "\n"
proc (count-down-from n) {
	\ () {
		! n (- n 1)
	}
} 
set count-down-from-4 (count-down-from 4)
puts "count-down-from-4 stores its status" "\n"
puts (count-down-from-4) "\n"
puts (count-down-from-4) "\n"
puts (count-down-from-4) "\n"
puts "\n"

puts "** partial evaluation **" "\n" "\n"
proc (two_params x y) {
	+ y x
}
puts "two_params with... two params = " (two_params 2 3) "\n"
puts "but if you call it with one param you get... " 
puts (two_params 3) "\n" "\n"

puts "that's all folks!" "\n"

# eof
