
# --------------------------------
# The f8 (fate) scripting language
# --------------------------------
#
# Example: dimensionality reduction with PCA
#
# (c) www.carminecella.com
#

source "stdlib.tcl"
source "learning.tcl"
source "plotting.tcl"

set f (openstream "../data/iris.data.txt" 'input 'text)
set orig (readcsv f)
set orig_sh (lshuffle orig)
set data (make-dataset orig_sh)

set samples (llength data)
set features (size (car (car data)))

set i 0
set matrix ()
set labels ()
while (< i samples) {
    lappend matrix (car (car (lrange data i 1)))
    lappend labels (car (cdr (car (lrange data i 1))))    
    = i (+ i 1)
}

set m (cols matrix)
set n (rows matrix)
puts "data " m "x" n ": \n"
matdisp matrix
puts "\n" 

set eigen (pca matrix) 
puts "full eigenvectors and eigenvalues:\n"
matdisp eigen

set dimensions 2
puts "reduced eigenvectors:\n"
set evect (getcols eigen 0 (- dimensions 1))
matdisp evect

puts "reduced data:\n"
set red (matmul matrix evect)
matdisp red   

set colors ()
set i 0
while (< i (llength labels)) {
    if (eq (car (lrange labels i 1)) 'Iris-virginica) (lappend colors Blue)
    if (eq (car (lrange labels i 1)) 'Iris-versicolor) (lappend colors Red)
    if (eq (car (lrange labels i 1)) 'Iris-setosa) (lappend colors Green)
    = i (+ i 1)
}

set g (opensvg "iris_reduced.svg" 512 512)
scatter g (array (getcols red 0 0)) (array (getcols red 1 1)) colors
title g "PCA reduction on IRIS"
legend g "virginica" Blue "versicolor" Red "setosa" Green
grid g
closesvg g

# eof 

