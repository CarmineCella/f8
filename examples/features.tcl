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
set f0 ()
set nrg ()
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
    assign oamps amps 0 N

    # time domain
    lappend f0 (acorrf0 (slice sig (* i hop) N) SR)
    lappend nrg (energy (slice sig (* i hop) N))
    = i (+ i 1)
}

set g (opensvg "spec_features.svg" 512 512)
polyline g (array centr) Blue
polyline g (array spread) Green
polyline g (array skew) Cyan
polyline g (array kurt) Black
polyline g (array flux) Red
title g "Spectral features"
legend g "centroid" Blue "spread" Green "skewness" Cyan "kurtosis" Black "flux" Red
grid g 0 (/ (size sig) SR)
closesvg g

= f0 (median (array f0) 5)
= g (opensvg "f0.svg" 512 512)
polyline g f0 Purple
title g "Fundamental frequency"
legend g "autocorr f0" Purple
grid g 0 (/ (size sig) SR) 0 (/ SR 2)
closesvg g

= g (opensvg "energy.svg" 512 512)
polyline g (array nrg) Blue
title g "Energy"
grid g 0 (/ (size sig) SR)
closesvg g

set f0s (array (map (\(x)(dup hop x)) (array2list f0)))
set amps (array (map (\(x)(dup hop x)) nrg))

set t1 (gen 4096 1)
set synth (* amps (osc SR f0s t1))

sndwrite "f0_synth.wav" SR (list synth)

# eof

