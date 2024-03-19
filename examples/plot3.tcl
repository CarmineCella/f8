# --------------------------------
# The f8 (fate) scripting language
# --------------------------------
#
# Example: scatter plots
#
# (c) www.carminecella.com
#

source "stdlib.tcl"
source "plotting.tcl"

set a (birandn 30 64)
set b (birandn 30 64)

set s0 (array (-> cos (map (\(x)(/ x 32 )) (array2list (bpf 0 256 256)))))

set g (opensvg "scatter.svg" 512 512)
scatter g a b Blue
polyline g s0 Red
title g "Scatter plot"
closesvg g


# eof
