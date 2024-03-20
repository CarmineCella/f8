# --------------------------------
# The f8 (fate) scripting language
# --------------------------------
#
# Example: onset detection
#
# (c) www.carminecella.com
#

source "stdlib.tcl"
source "signals.tcl"
source "plotting.tcl"

set N 4096
set hop (/ N 4)
set SR 44100
set frame (/ N SR)
set threshold 0.5
set timegate 0.1

set w (car (sndread "../data/onsets.wav"))

set o (onsets w SR N hop threshold timegate)
puts o "\n"

set imp (zeros (size w))

set i 0
while (< i (llength o)) {
    set curr (lindex o i)
    puts (size imp) (car curr) "\n"
    setval imp (second curr) (+ hop (* hop (car curr)))
    = i (+ 1 i)
}

set g (opensvg "onsets.svg" 512 512)
polyline g w Blue
polyline g imp Red
title g "Onset detection"
legend g "signal" Blue "onsets" Red
closesvg g

# eof

