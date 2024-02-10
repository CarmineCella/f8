# --------------------------------
# The f8 (fate) scripting language
# --------------------------------
#
# Example: organising sounds in a timeline
#
# (c) www.carminecella.com
#

source "stdlib.tcl"

proc (oscbank sr amps freqs tab) {
    # assumes both freqs and amps have the same number of elems
	set elems (llength amps) 
	if (not (eq elems 0)) {
		set outbuff (bpf 0 (size (car freqs)) 0)
		set i 0
		while (< i elems) {
			assign outbuff (+ outbuff (* (car amps) (osc sr (car freqs) tab))) 0 (size outbuff)
			set amps (cdr amps)
			set freqs (cdr freqs)
			set i (+ i 1)
		}
		+ outbuff
	}
}

set sr 44100
set samps (* 30 sr)

set tab1 (gen 4096 1 0.5 0.25 0.125)
set tab2 (gen 4096 1)

set amps (list (bpf 0 (/ samps 3) 0.3 (/ samps 3) 0.3 (/ samps 3) 0) (bpf 0 (/ samps 2) 0.3 (/ samps 2) 0) (bpf 0 (/ samps 2) 0.3 (/ samps 2) 0))
set freqs (list (bpf 220 samps 550) (bpf 110 samps 220) (bpf 440 samps 220))

set env (bpf 0 (/ samps 2) 0.5 (/ samps 2) 0)

set s1 (* env (osc sr (bpf 220 samps 330) tab1))
set s2 (* env (osc sr (bpf 220 samps 1660) tab1))
set s3 (oscbank sr amps freqs tab2)
set s4 (* env (osc sr (bpf 110 samps 60) tab1))

set timeline1 (mix 0 s1 (/ samps 2) s2 (* 3 (/ samps 4)) s3 0 s4))

set ir_file  (openwav "../sounds/Concertgebouw-s.wav" 'input)
set ir (readwav ir_file)  
closewav ir_file

set irL (car ir)
set irR (second ir)

set scale 0.01
set mix 0.4

set outsigL (conv irL timeline1 scale mix)
set outsigR (conv irR timeline1 scale mix)

set out (openwav "timelines.wav" 'output sr 2)
writewav out outsigL outsigR
closewav out

# eof


