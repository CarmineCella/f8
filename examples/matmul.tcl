# --------------------------------
# The f8 (fate) scripting language
# --------------------------------
#
# Example: matrix operations
#
# (c) www.carminecella.com
#

set a (noise 8800 1024)
set b (noise 400 8800)
set c (noise 5 3)

set d (matmul a b )

# puts "input matrices:\n"
# matdisp a
# matdisp b
# # matdisp c

# puts "matrix multiplication:\n"
# matdisp d

