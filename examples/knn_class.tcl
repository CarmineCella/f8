# --------------------------------
# The f8 (fate) scripting language
# --------------------------------
#
# Example: a simple KNN classifier
#
# (c) www.carminecella.com
#

source "stdlib.tcl"

set f (openstream "../data/iris.data.txt" 'input 'text)
set orig (readcsv f)
set data (lshuffle orig)

set samples (llength data)
set features (- (llength (car data)) 1)

puts samples " samples - " features " features\n"

set trainsz (floor (* 80 (/ samples 100)))
set testsz (- samples trainsz)
set trainset (lrange data 0 trainsz)
set testset (lrange data trainsz testsz)

puts "split: " (llength trainset) " train samples, " (llength testset) " test samples\n"

puts "training..."
set model (knntrain trainset 3)
puts "done\n"

set classes (knntest model testset)

puts "accuracy = " (accuracy classes testset) "\n"

# eof
