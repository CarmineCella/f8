# --------------------------------
# The f8 (fate) scripting language
# --------------------------------
#
# Signal processing library
#
# (c) www.carminecella.com
#

source "stdlib.tcl"

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
proc (peaks f) {
	set p ()
	if (> (size f) 2) {
		set i 1
		while (< i (- (size f) 1)) {
			set curr (getval f i)
			set prev (getval f (- i 1))
			set next (getval f (+ i 1))
			if (and (> curr prev) (> curr next)) {
				lappend p i
			}
			= i (+ 1 i)
		}
	}
	set p
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
proc (onsets sig sr N hop threshold timegate) {
	set data (car (stft sig N hop))
	set i 0
	set frame (/ hop sr)
	set flux ()
	set oamps (zeros N)
	while (< i (llength data)) {
		set magphi (car2pol (lindex data i))
		set magphi_d (deinterleave magphi)
		set amps (car magphi_d)

		lappend flux (specflux amps oamps)
		assign oamps amps 0 N
		= i (+ i 1)
	}	
	= flux (array flux)
	set p (peaks flux)
	= i 0
	set ons ()
	set prev_on 0
	while (< i (llength p)) {
		set pos (lindex p i)
		set f (getval flux pos)
		set t (* pos frame)
		set delta (- t prev_on)
		if (or (and (> f threshold) (> delta timegate)) (eq prev_on 0)) {
			lappend ons (list pos f)
			= prev_on t
		}
		= i (+ 1 i)
	}
	set ons
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
proc (makedict folder) {
	set files (dirlist folder)
	set i 0
	set db ()
	while (< i (llength files)) {
		set f (lindex files i)
		if (>= (string 'find f ".wav") 0) {
			# TODO: how to handle different platforms?
			set sig (sndread (tostr folder "/" f))
			set noext (string 'replace f ".wav" "")			
			set tokens (string 'split noext "-")
			lappend db (list sig f tokens)
			# lappend db (list (tostr folder "/" f) tokens)
		}
		= i (+ i 1)
	}

	= db (lshuffle db)
}

# eof

