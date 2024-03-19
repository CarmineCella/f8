# --------------------------------
# The f8 (fate) scripting language
# --------------------------------
#
# Signal processing library
#
# (c) www.carminecella.com
#

 proc (oscbank sr amps freqs tab) {
    # assumes both freqs and amps have the same number of elems
	set elems (llength amps) 
	set outbuff (bpf 0 (size (car freqs)) 0)
	set i 0
	while (< i elems) {
		set f (car freqs)
		if (eq f ()) {break}
		= outbuff (+ outbuff (* (car amps) (osc sr (car freqs) tab)))
		set amps (cdr amps)
		set freqs (cdr freqs)
		= i (+ i 1)
	}
	set outbuff
}
proc (sndread fname) {
    set h (openwav fname 'input)
    set b (readwav h)
    closewav h
    set b
}
proc (sndwrite fname sr data) {
    set ch (llength data)
    set h (openwav fname 'output sr ch)
    set b (writewav h data)
    closewav h
    set b
}

# eof

