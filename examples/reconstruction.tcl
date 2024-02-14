# --------------------------------
# The f8 (fate) scripting language
# --------------------------------
#
# Example: sound analysis and reconstruction
#
# (c) www.carminecella.com
#

source "stdlib.tcl"

set sr 44100
set dur 1.2
set samps (* dur sr)

set nyquist (/ sr 2)
set nwin 4096
set offset 128

set tab1 (gen nwin 1)

puts "analysing......"
set a (openwav "../data/gong_c#3.wav" 'input)
set input (car (readwav a))
closewav a

set spec (fft input)
set polar (car2pol spec)
set mag (slice polar 0 (/ (size polar) 2) 2)
set mag (slice mag offset nwin)

set fftfreqs (bpf 0 (/ (size spec) 2) nyquist)
set fftfreqs (slice fftfreqs offset (size mag))

set amps ()
set i 0
while (< i (size mag)) {
	set v (slice mag i 1)
    lappend amps (bpf v samps v)
    ! i (+ i 1)
}
set freqs (list)
! i 0
while (< i (size fftfreqs)) {
    set v (slice fftfreqs i 1)
    lappend freqs (bpf v samps v)
    set i (+ i 1)
}

puts "done\n" "synthesising..."
set out (oscbank sr amps freqs tab1)
set norm 0.8
set fade (bpf norm samps 0.0)
# set out (* fade out)
puts "done\n"

set c (openwav "reconstruction.wav" 'output sr 1)
puts (writewav c (list out)) " samples written\n"
closewav c

# eof
