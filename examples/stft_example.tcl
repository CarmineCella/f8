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
set sig (istft spec N hop)

sndwrite "stft_rebuild.wav" SR (list sig)

# eof

