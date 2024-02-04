# {plot}

source "stdlib.tcl"
set a (birandn 30 64)
set b (birandn 30 64)

set s0 (-> cos (map (\(x)(/ x 32 )) (bpf 0 256 256)))

set g (opensvg "scatter.svg" 512 512)
scatter g a b Blue
polyline g s0 Red
title g "Scatter plot"
closesvg g


# eof
