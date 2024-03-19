# temp: SVD tests

source "stdlib.tcl"
source "learning.tcl"

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
matdisp matrix

set usvt (svd matrix) 

set u (car usvt)
set s (second usvt)
set vt (third usvt)

matdisp u
matdisp s
matdisp vt

