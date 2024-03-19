# --------------------------------
# The f8 (fate) scripting language
# --------------------------------
#
# Example: simple linear regression (line fit)
#
# (c) www.carminecella.com
#

source "stdlib.tcl"
source "plotting.tcl"

set x (bpf 1 50 50)
set y (+ (* x -0.3) (birandn 1 50))

set q (linefit x y)
puts "slope = " (getval q 0) ", intercept = " (getval q 1) "\n"

set g (opensvg "linear_regr.svg" 512 512)
scatter g x y Blue
polyline g (+ (bpf 0 50 (getval q 0)) (getval q 1)) Red
title g "A simple linear fitting"
closesvg g

# eof

