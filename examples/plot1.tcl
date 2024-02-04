# plot
source "stdlib.tcl"
set a (opensvg "test.svg" 512 512)
set s0 (-> cos (map (\(x)(/ x 32 )) (bpf 0 256 256)))
set s1 (-> sin (map (\(x)(/ x 16 )) (bpf 0 128 128)))
set s2 (-> sqrt (bpf 0 1024 1))
set s3 (map (\(x)(* x x))(bpf 0 1024 1))
polyline a s0 Aqua
polyline a s1 Blue
polyline a s2 Red
polyline a s3 Green
grid a 0 1024 -1 1
legend a "cosine" Aqua "sine" Blue "sqrt" Red "pow2" Green
title a "This is a plot!"
closesvg a
