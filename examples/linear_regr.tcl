# --------------------------------
# The f8 (fate) scripting language
# --------------------------------
#
# Example: simple linear regression
#
# (c) www.carminecella.com
#

source "stdlib.tcl"

set w (bpf 1 50 50)
set p (+ (* w -0.3) (birandn 1 50))

set q (linreg w p)
puts "slope = " (getval q 0) ", intercept = " (getval q 1) "\n"

set g (opensvg "linear_regr.svg" 512 512)
scatter g p w Blue
polyline g (+ (bpf 0 50 (getval q 0)) (getval q 1)) Red
closesvg g

# eof

