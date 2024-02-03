# Overview of some features of f8
#
#

source "stdlib.tcl"

puts "** hallo f8!! **" nl nl

puts "** inrtospection **" nl nl
puts "the list of current variables is: " nl
puts (info 'vars) nl nl
puts "the list of current variables starting with letter l is: " nl
puts (info 'vars "l.*") nl nl
puts "there are several types, among which: " nl
puts '(a b c d) " = " (info 'typeof '(a b c d)) nl
puts 'a_symbol " = " (info 'typeof 'a_symbol) nl
puts "a simple string = " (info 'typeof "a simple string") nl
puts 4  " = " (info 'typeof 4) nl
puts "map = "(info 'typeof map) nl 
puts + " = " (info 'typeof +) nl nl

puts "** scopes are local to procs **" nl nl
puts "define proc alpha and proc beta inside it" nl
proc (alpha x) {
	proc (beta x) {
		puts x
	}
	
	puts "call beta from alpha: "
	beta 4
}
puts (alpha 5) nl
puts "does beta exist here? " (info 'exists 'beta) nl nl

puts "** local scope **" nl nl
let {
	set k 100
	puts "local variable k = " k nl
}
puts "does k exists here? " (info 'exists 'k) nl nl

puts "** dynamic scope **" nl nl
set b 5
puts "define dynamic foo" nl 
dynamic (foo) {
   set a (+ b 5)
}
puts "define static bar" nl 
proc (bar) {
   set b 2
   foo
}
puts "since foo is a dynamic procedure and bar is a static procedure:" nl
puts "foo: " foo nl "bar: " bar nl
puts "when you call it you get " (foo) nl
puts "buf if you call it through bar instead you get " (bar) nl nl

puts "** while loops with flow control **" nl nl
set b 10
puts "start from " b nl
if (> b 5) {
	set b (+ b 10)
	puts "this is " b " now... go higher!" nl
} else {
	puts "not executed :/" nl
}
puts "a loop..." nl
while (> b 0) {
	set b (- b 1)
	if (eq b 5) {
		puts "here we stop at " b nl
		break
	} 
	puts b nl
}
puts nl

puts "** recursion **" nl nl
proc (bad_fib x) {
    if (<= x 1) 1 else {
        + (bad_fib (- x 1)) (fib (- x 2))
    }
}
puts "fibonacci of 20 is " (bad_fib 20) nl
puts nl

puts "** higher functions with lambdas **" nl nl
puts "map " (map (\(x)(* x x)) '(1 2 3 4 5)) nl
puts "filter " (filter (\ (x) (> x 5)) '(5 2 11 -7 8 1)) nl
puts "unpack " (unpack + (list 4 5)) nl nl

puts "** closures **" nl nl
proc (count-down-from n) {
	\ () {
		! n (- n 1)
	}
} 
set count-down-from-4 (count-down-from 4)
puts "count-down-from-4 stores its status" nl
puts (count-down-from-4) nl
puts (count-down-from-4) nl
puts (count-down-from-4) nl
puts nl

puts "** partial evaluation **" nl nl
proc (two_params x y) {
	+ y x
}
puts "two_params with... two params = " (two_params 2 3) nl
puts "but if you call it with one param you get... " 
puts (two_params 3) nl nl

puts "that's all folks!" nl
