# sound analysis and reconstruction

source "stdlib.tcl"

set sr 44100
set dur 1.2
set samps (* dur sr)
set nyquist (/ sr 2)
set nwin 4096
set nwin2 (/ nwin 2)
set hop 128

set tab1 (gen nwin 1)

puts "analysing......"
set a (openwav "../sounds/gong_c#3.wav" 'input)
set input (car (readwav a))
closewav a
set spec (fft input)

set polar (car2pol spec)
set mag (slice polar 0 (/ (size polar) 2) 2)

set mag (slice mag hop nwin2)

set fftfreqs (bpf 0 (/ (size spec) 2) nyquist)
set fftfreqs (slice fftfreqs hop (size mag))

set amps (list)
set i 0
while (< i (size 2)) {
    set v (lindex i mag)
    lappend amps (bpf v samps v)
    set i (+ i 1)
}

set freqs (list)
! i 0
while (< i (size 2)) {
    set v (lindex i fftfreqs)
    lappend freqs (bpf v samps v)
    set i (+ i 1)
}

proc (oscbank sr amps freqs tab) {
    #  assumes both freqs and amps have the same number of elems
	set elems (llength amps)
	if elems {
		set outbuff (bpf 0 (size (car freqs)) 0)
		set i 0
        puts (info 'typeof (car freqs)) nl
		while (< i elems) {
            set sines (osc sr (car freqs) tab)
			! outbuff (+ outbuff (* (car amps)))
			set amps (cdr amps)
			set freqs (cdr freqs)
			set i (+ i 1)
		}
		+ outbuff 0
	}
}

puts "done" nl "synthesising..."
set out (oscbank sr amps freqs tab1)
set norm 0.8

set fade (bpf norm samps 0.0)
set out (* fade out)
puts "done" nl

set c (openwav "reconstruction.wav" 'output sr 1)
puts (writewav c 1 out) " samples written" nl
closewav c
