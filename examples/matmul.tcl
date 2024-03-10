# --------------------------------
# The f8 (fate) scripting language
# --------------------------------
#
# Example: matrix operations
#
# (c) www.carminecella.com
#

set a (noise 8 4)
set b (noise 5 8)
set c (noise 3 5)

set d (matmul a b c)

puts "input matrices:\n"
matdisp a
matdisp b
matdisp c

puts "matrix multiplication:\n"
matdisp d

