# --------------------------------
# The f8 (fate) scripting language
# --------------------------------
#
# Example: a simple KNN classifier
#
# (c) www.carminecella.com
#

source "stdlib.tcl"
source "learning.tcl"

set f (openstream "../data/iris.data.txt" 'input 'text)
set orig (readcsv f)
set orig_sh (lshuffle orig)
set data (dataset orig_sh)

set samples (llength data)
set features (size (car (car data)))
puts samples " samples - " features " features\n\n"

set trainsz (floor (* 80 (/ samples 100)))
set testsz (- samples trainsz)
set trainset (lrange data 0 trainsz)
set testset (lrange data trainsz testsz)

puts "split: " (llength trainset) " train samples, " (llength testset) " test samples\n\n"

puts "training..."
set model (knntrain trainset 3)
puts "done\n"

puts "testing..."
set classes (knntest model testset)
puts "done\n\n"

puts "accuracy = " (accuracy classes testset) "\n"

# eof
