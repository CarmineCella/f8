# --------------------------------
# The f8 (fate) scripting language
# --------------------------------
#
# Example: mathmetical notation, arrays, dotted/applied operators
#
# (c) www.carminecella.com
#

source "stdlib.tcl"

puts "ordinary operators (on arrays)...\n"
set a '(+ 1 2 3 4)
puts a " = " (eval a) "\n"

set a '(+ (array 1 2 3) (array 4 5 6))
puts a " = " (eval a) "\n"

set a '(+ (array 1 2 3) 4)
puts a " = " (eval a) "\n"

set a '(+ 4 (array 1 2 3))
puts a " = " (eval a) "\n"

set a '(slice (array 1 2 3 4 5 6 7 8) 0 5 2)
puts a " = " (eval a) "\n"

set a '(slice (array 1 2 3 4 5 6 7 8) 1 5 2)
puts a " = " (eval a) "\n"

set a '(assign (array 1 2 3 4 5 6 7 8) (slice (array 1 2 3 4 5 6 7 8) 1 5 2) 0 5 2)
puts a " = " (eval a) "\n"

set a '(assign (array 1 2 3 4 5 6 7 8) (slice (array 1 2 3 4 5 6 7 8) 0 5 2) 1 5 2)
puts a " = " (eval a) "\n"

set a '(sum (array 1 2 3 4))
puts a " = " (eval a) "\n"

set a '(min (array 1 2 3 4))
puts a " = " (eval a) "\n"

set a '(max (array 1 2 3 4))
puts a " = " (eval a) "\n"

set a '(dot (array 1 2 3) (array 4 5 6))
puts a " = " (eval a) "\n"

set a '(getval (array 1 2 3 4) 2)
puts a " = " (eval a) "\n"

set a '(setval (array 1 2 3 4) 100 2)
puts a " = " (eval a) "\n"

puts "\ndotted/applied operators (on lists)...\n"
set a '(-> + '(1 2 3 4))
puts a " = " (eval a) "\n"

set a '(map neg '(1 2 3 4))
puts a " = " (eval a) "\n"

set a '(. + '(1 2 3) '(4 5 6))
puts a " = " (eval a) "\n"

set a '(map2 (\(x y) (+ x y)) '(1 2 3) '(4 5 6))
puts a " = " (eval a) "\n"

set a '(sign '(-3 -4 1 2 -5 -6 7))
puts a " = " (eval a) "\n"

puts "\nfrom arrays to lists and viceversa...\n"
set a '(+ (array '(1 2 3)) (array '(4 5 6)))
puts a " = " (eval a) "\n"

set a '(array2list (+ (array '(1 2 3)) (array '(4 5 6))))
puts a " = " (eval a) "\n"

set a '(array (. + '(1 2 3) '(4 5 6)))
puts a " = " (eval a) "\n"

set a '(array 1 2 3 (list 5 6 7 (array 8 9)) 10)
puts a " = " (eval a) "\n"

set a '(array2list (array 1 2 3 (list 5 6 7 (array 8 9)) 10))
puts a " = " (eval a) "\n"

set a '(size (array 1 2 3 (list 5 6 7 (array 8 9)) 10))
puts a " = " (eval a) "\n"

set a '(llength (array2list (array 1 2 3 (list 5 6 7 (array 8 9)) 10)))
puts a " = " (eval a) "\n"

# eof

