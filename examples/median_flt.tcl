# --------------------------------
# The f8 (fate) scripting language
# --------------------------------
#
# Example: median filtering
#
# (c) www.carminecella.com
#

source "stdlib.tcl"

set sr 100
set t (bpf 0 sr 1)

set x (+ (sin (* t TWOPI 3)) (* (sin (* t TWOPI 40)) 0.25))
set y (median x 10)

puts (size y) " " (size x) "\n"

set g (opensvg "median_denoise.svg" 512 512)
polyline g x Blue
polyline g y Red
legend g "noisy signal" Blue "filtered signal" Red
title g "Median filtering"

closesvg g

# eof



