# --------------------------------
# The f8 (fate) scripting language
# --------------------------------
#
# Signal processing library
#
# (c) www.carminecella.com
#

proc (bartlett N) {
	bpf 0 (/ N 2) 1 (/ N 2) 0
} 
proc(hanning N) {
	window N 0.5 0.5 0
}
proc(hamming N) {
	window N 0.54 0.46 0
}
proc(blackman N) {
	window N 0.42 0.5 0.08
}
proc (stft sig N hop) {
	set c 0
	set len (size sig)
	set spec ()
	set win (hanning N)
	while (< c len) {
		set buff (* win (slice sig c N))
		lappend spec (fft buff)
		= c (+ c hop)
	}
	list spec len
}
proc (istft spec N hop) {
	set i 0
	set data (car spec)
	set sz (second spec)
	set len (llength data)
	set win (hanning N)
	set out ()
	while (< i len) {
		set buff (* win (ifft (lindex data i)))
		lappend out (* i hop) buff
		= i (+ i 1)
	}
	slice (-> mix out) 0 sz
}
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

