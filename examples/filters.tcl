
# --------------------------------
# The f8 (fate) scripting language
# --------------------------------
#
# Example: basic audio filters
#
# (c) www.carminecella.com
#

source "stdlib.tcl"
source "signals.tcl"

set sr 44100
set order 51
set w (car (sndread "../data/cage.wav"))

puts "lowpass: "
set cutoff 200
set h (firdesign 'lowpass order sr cutoff)
puts  h "\n"
set f (conv w h 1)
sndwrite "lp.wav" sr (list f)

puts "bandpass: "
set cutoff 2000
set cutoff_high 2500
set order 120
set h (firdesign 'bandpass order sr cutoff cutoff_high)
puts  h "\n"
set f (conv w h 1)
sndwrite "bp.wav" sr (list f)

# TODO: reson, biqud, onepole...

# eof

