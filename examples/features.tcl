# --------------------------------
# The f8 (fate) scripting language
# --------------------------------
#
# Example: low-leve audio features
#
# (c) www.carminecella.com
#

source "stdlib.tcl"
source "signals.tcl"

set a (noise 1024)
set s (stft a 512 256)
set data (car s)

set magphi (map car2pol data)
puts (llength (second ap)) "\n"

# eof

