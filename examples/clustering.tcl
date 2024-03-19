
# --------------------------------
# The f8 (fate) scripting language
# --------------------------------
#
# Example: clustering with K-means
#
# (c) www.carminecella.com
#

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
set olabels ()
while (< i samples) {
    lappend matrix (car (car (lrange data i 1))) 
    lappend olabels (car (cdr (car (lrange data i 1))))    
    = i (+ i 1)
}

set K 3
set clusters (kmeans matrix K)
set labels (array2list (car clusters))
set centroids (second clusters)

puts "centroids:\n"
matdisp centroids

set i 0
while (< i (llength labels)) {
    puts (car (lrange labels i 1)) " = " (car (lrange olabels i 1)) "\n"
    = i (+ i 1) 
}

# eof 

