# --------------------------------
# The f8 (fate) scripting language
# --------------------------------
#
# Example: plotting functionalities
#
# (c) www.carminecella.com
#

source "stdlib.tcl"

set a (opensvg "test.svg" 512 512)

set s0 (cos (bpf 0 256 32))
set s1 (sin (bpf 0 128 16))
set s2 (sqrt (bpf 0 1024 1))
# example of interoperability between numeric and list
set s3 (toarray (map (\(x)(* x x)) (tolist (bpf 0 1024 1))))

polyline a s0 Aqua
polyline a s1 Blue
polyline a s2 Red
polyline a s3 Green
grid a 0 1024 -1 1
legend a "cosine" Aqua "sine" Blue "sqrt" Red "pow2" Green
title a "This is a plot!"

closesvg a

# eof
