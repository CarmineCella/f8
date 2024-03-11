# --------------------------------
# The f8 (fate) scripting language
# --------------------------------
#
# Example: matrix operations
#
# (c) www.carminecella.com
#

set a (noise 180 4)
set b (noise 5 180)
set c (noise 3 5)

set d (matmul a b c)

puts "input matrices:\n"
matdisp a
matdisp b
matdisp c

puts "matrix multiplication:\n"
matdisp d

puts "transposition of the result:\n"
matdisp (transp d)

puts "selection (to make it square):\n"
set sub_d (getrows d 0 2)
matdisp sub_d   

puts "determinant: " (det sub_d) "\n\n"

puts "inverted matrix:\n"
set inv_sub_d (inv sub_d)
matdisp inv_sub_d

puts "sum across rows: " (matsum inv_sub_d 0) "\n"
puts "sum across cols: " (matsum inv_sub_d 1) "\n\n"

puts "compute PCA:\n"
set p (pca inv_sub_d)
matdisp p
puts "only eigenvalus: " (getcols p 3 3) "\n"

puts "id matrix:\n"
matdisp (eye 5)

# eof

