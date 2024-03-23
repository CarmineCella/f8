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
set frame (/ N SR)

set w (sndread "../data/Noisy_beka.wav")
set spec (stft (car w) N hop)
set magphi (map car2pol (car spec))
set magphi_d (map deinterleave magphi)
set amps (map car magphi_d)
set phis (map second magphi_d)
# denoising
= amps (map (\(x)(* (> x 0.001) x)) amps)
= magphi_d (zip amps phis)
set magphi_i (map interleave magphi_d)
set imre (map pol2car magphi_i)
set spec2 (list imre (second spec))

set sig (istft spec2 N hop)

sndwrite "stft_rebuild.wav" SR (list sig)

# eof

