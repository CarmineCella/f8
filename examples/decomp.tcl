source "stdlib.tcl"
source "signals.tcl"
source "learning.tcl"
source "plotting.tcl"

# set data (make-dictionary "../../../From_scratch/samples")
set data (make-dictionary "../data")

set target (car (sndread "../data/cage.wav"))

set dec (mpdecomp target 0 10 data)

set w (mprebuild (car dec) data)
sndwrite "rebuild.wav" 44100 (map (\(x)(* x 100)) w)
