# --------------------------------
# The f8 (fate) scripting language
# --------------------------------
#
# Example: short-time Fourier transform
#
# (c) www.carminecella.com
#

source "stdlib.tcl"
source "signals.tcl"

set N 4096
set hop (/ N 4)
set SR 44100

set w (sndread "../data/cage.wav")
set spec (stft (car w) N hop)
set magphi (map car2pol (car spec))
set imre (map pol2car magphi)
set spec2 (list imre (second spec))
set sig (istft spec2 N hop)

sndwrite "stft_rebuild.wav" SR (list sig)

# eof

