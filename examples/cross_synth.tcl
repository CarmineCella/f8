# --------------------------------
# The f8 (fate) scripting language
# --------------------------------
#
# Example: cross-synthesis in frequency domain
#
# (c) www.carminecella.com
#

source "stdlib.tcl"
source "signals.tcl"

set sr 44100
set hop 512
set sz 2048

set sig_file (openwav "../data/Vox.wav" 'input)
set ir_file  (openwav "../data/Beethoven_Symph7.wav" 'input)

set sig1 (car (readwav sig_file))
set sig2 (car (readwav ir_file))

set sig1_len (size sig1)
set sig2_len (size sig2)
set min_len (size sig1)

if (< sig2_len sig1_len) (set min_len sig2_len)
set min_len (- min_len sz)

puts "sig 1 len = " (size sig1) ", sig 2 len = " (size sig2) ", min len = " min_len "\n"

set i 0
set bwin (hanning sz)
set outsig (bpf 0 (+ sz min_len) 0)

# denoise threshold
set threshold (bpf 0.0001 (/ sz 2) 0.0001) 

proc (interleave in1 in2) {
    set out (bpf 0 (+ (size in1) (size in2)) 0)
    assign out in1 0 (size in1) 2
    assign out in2 1 (size in2) 2
}

while (< i min_len) {
    set buff1 (slice sig1 i sz)
    set buff1 (* bwin buff1)
    set spec1 (car2pol (fft buff1))
    set amps1 (slice spec1 0 (/ (size spec1) 2) 2)
    set phi1  (slice spec1 1 (/ (size spec1) 2) 2)
    set amps1 (* (> amps1 threshold) amps1)

    set buff2 (slice sig2 i sz)
    set buff2 (* bwin buff2)
    set spec2 (car2pol (fft buff2))
    set amps2 (slice spec2 0 (/ (size spec2) 2) 2)
    set phi2  (slice spec2 1 (/ (size spec2) 2) 2)

    set outamps (sqrt (* amps1 amps2))
    set outphi phi2
    set outbuff (ifft (pol2car (interleave outamps outphi)))
    set outbuff (* bwin outbuff)

    assign outsig (+ (slice outsig i (size outbuff)) outbuff) i (size outbuff)
    set i (+ i hop)
}
puts  "min = " (min outsig) ", max = " (max outsig) "\n"

set outf (openwav "xsynth.wav" 'output sr 1)
puts (writewav outf (list outsig)) " samples written\n"
closewav outf

# eof

