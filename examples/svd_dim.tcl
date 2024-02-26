source "stdlib.tcl"

set f (openstream "../data/iris.data.txt" 'input 'text)
set orig (readcsv f)
set orig_sh (lshuffle orig)
set data (make-dataset orig_sh)

set samples (llength data)
set features (size (car (car data)))

set i 0
set matrix ()
while (< i samples) {
    lappend matrix (car (car (lrange data i 1)))
    = i (+ i 1)
}

puts "data:\n"
dispmat matrix
puts "\n"

set usvt (svd matrix) 
puts "singular values:\n"
puts (second usvt) "\n"