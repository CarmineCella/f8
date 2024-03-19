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
source "plotting.tcl"

set N 4096
set hop (/ N 4)
set SR 44100
set frame (/ N SR)

set sig (car (sndread "../data/Vox.wav"))
set s (stft sig N hop)
set data (car s)

set freqs (bpf 0 N SR)
set i 0
set oamps (zeros N)
set centr ()
set spread ()
set skew ()
set kurt ()
set flux ()
set irr ()
set decr ()
set f0b ()
set nrg ()
set zx ()
set c 0
set s 0
while (< i (llength data)) {
    set magphi (car2pol (lindex data i))
    set magphi_d (deinterleave magphi)
    set amps (car magphi_d)
    set phis (second magphi_d)

    # freq domain
    = c (speccent amps freqs)
    lappend centr c
    = s (specspread amps freqs c)
    lappend spread s
    lappend skew (specskew amps freqs c s)
    lappend kurt (speckurt amps freqs c s)
    lappend flux (specflux amps oamps)
    lappend irr (specirr amps)
    lappend decr (specdecr amps)
    assign oamps amps 0 N

    # time domain
    lappend f0b (acorrf0 (slice sig (* i hop) N) SR)
    lappend nrg (energy (slice sig (* i hop) N))
    lappend zx (zcr (slice sig (* i hop) N))
    = i (+ i 1)
}

set g (opensvg "spec_features.svg" 512 512)
polyline g (array centr) Blue
polyline g (array spread) Green
polyline g (array skew) Cyan
polyline g (array kurt) Black
polyline g (array flux) Red
polyline g (array irr) Purple
polyline g (array decr) Gray
title g "Spectral features"
legend g "centroid" Blue "spread" Green "skewness" Cyan "kurtosis" Black "flux" Red "irregulairty" Purple "decrease" Gray
grid g 0 (/ (size sig) SR)
closesvg g

= f0b (median (array f0b) 5)
= g (opensvg "f0.svg" 512 512)
polyline g f0b Purple
polyline g (array zx) Blue
title g "Fundamental frequency"
legend g "autocorr f0" Purple "zero-crossing" Blue
grid g 0 (/ (size sig) SR) 0 (/ SR 2)
closesvg g

= g (opensvg "energy.svg" 512 512)
polyline g (array nrg) Blue
title g "Energy"
grid g 0 (/ (size sig) SR)
closesvg g

set f0bl (array2list f0b)
set f0s (array (map2 (\(x y)(bpf x hop y)) f0bl (ldrop f0bl 1)))
set amps (array (map2 (\(x y)(bpf x hop y)) nrg (ldrop nrg 1)))

set t1 (gen 4096 1 0.73 0.6 0.20)
set synth (* amps (osc SR f0s t1))

sndwrite "f0_synth.wav" SR (list synth)

# eof

