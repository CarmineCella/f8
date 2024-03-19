

source "stdlib.tcl"
source "signals.tcl"

set a (noise 1024)
set s (stft a 512 256)
set data (car s)

set magphi (map car2pol data)
puts (llength (second ap)) "\n"

