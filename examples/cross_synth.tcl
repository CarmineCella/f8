source "stdlib.tcl"

set sr 44100
set hop 512
set sz 2048

set sig_file (openwav "../sounds/Vox.wav" 'input)
set ir_file  (openwav "../sounds/Beethoven_Symph7.wav" 'input)

set sig1 (first (readwav sig_file))
set sig2 (first (readwav ir_file))

set sig1_len (asize sig1)
set sig2_len (asize sig2)
set min_len (asize sig1)

if (< sig2_len sig1_len) {set min_len sig2_len}
set min_len (- min_len sz)

puts "sig 1 len = " (asize sig1) ", sig 2 len = " (asize sig2) ", min len = " min_len nl

set i 0
set bartlett (bpf 0 (/ sz 2) 1 (/ sz 2) 0)
set outsig (bpf 0 (+ sz min_len) 0)

# denoise threshold
set threshold (bpf 0.0001 (/ sz 2) 0.0001) 

proc (interleave in1 in2) {
    set out (bpf 0 (+ (asize in1) (asize in2)) 0)
    ! out (assign out in1 0 (asize in1) 2)
    ! out (assign out in2 1 (asize in2) 2)
}

while (< i min_len) {
    set buff1 (slice sig1 i sz)
    set buff1 (a* bartlett buff1)
    set spec1 (car2pol (fft buff1))
    set amps1 (slice spec1 0 (/ (asize spec1) 2) 2)
    set phi1  (slice spec1 1 (/ (asize spec1) 2) 2)
    # set amps1 (a* (> amps1 threshold) amps1)

    set buff2 (slice sig2 i sz)
    set buff2 (a* bartlett buff2)
    set spec2 (car2pol (fft buff2))
    set amps2 (slice spec2 0 (/ (asize spec2) 2) 2)
    set phi2  (slice spec2 1 (/ (asize spec2) 2) 2)

    set outamps (-> sqrt (a* amps1 amps2))
    
    set outphi phi2
    set outbuff (ifft (pol2car (interleave outamps outphi)))
    set outbuff (a* bartlett outbuff)
    ! outsig (assign outsig (a+ (slice outsig i (asize outbuff)) outbuff) i (asize outbuff))
    set i (+ i hop)
}

puts (amin outsig) " " (amax outsig) nl
set out (openwav "xsynth.wav" 'output 1 sr)
writewav out 1 outsig
closewav out
