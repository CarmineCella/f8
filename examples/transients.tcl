# --------------------------------
# The f8 (fate) scripting language
# --------------------------------
#
# Example: transient separation using stft
#
# (c) www.carminecella.com
#

source "stdlib.tcl"
source "signals.tcl"
source "learning.tcl"

set N 4096
set hop (/ N 4)
set SR 44100
set frame (/ N SR)

set w (sndread "../data/Gambale_cut.wav")
set spec (stft (car w) N hop)
set magphi (map car2pol (car spec))
set magphi_d (map deinterleave magphi)
set amps (map car magphi_d)
set phis (map second magphi_d)
set clusters (kmeans amps 2)
set labels (array2list (car clusters))

set j 0
set tamps ()
set samps ()
while (< j (llength labels)) {
    set t (* (lindex amps j) (- 1 (lindex labels j)))
    set s (* (lindex amps j) (lindex labels j))
    lappend tamps t
    lappend samps s
    = j (+ j 1)
}

= magphi_d (zip tamps phis)
set magphi_i (map interleave magphi_d)
set imre (map pol2car magphi_i)
set spec2 (list imre (second spec))

set sig (istft spec2 N hop)

sndwrite "component_1.wav" SR (list sig)

= magphi_d (zip samps phis)
set magphi_i (map interleave magphi_d)
set imre (map pol2car magphi_i)
set spec2 (list imre (second spec))

set sig (istft spec2 N hop)

sndwrite "component_2.wav" SR (list sig)

# eof

