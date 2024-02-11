# f8 - standard library
#

# true/false
set true 1
set false 0

# logical operators
proc (not x) (if x false else true)
proc (and x y)(if (if x true else false) (if y true else false) else false)
proc (or x y) (if x x else (if y y else false))
proc (<> n1 n2) (not (eq n1 n2))

# lists
proc (car l) (lindex l 0 )
proc (second l) (lindex l 1)
proc (head l) (lappend () (car l))
proc (cdr l) (lrange l 1 (- (llength l) 1))
proc (last l) (lindex l (- (llength l) 1))
proc (tail l) (lappend () (last l))
proc (take n l) {
	proc (take-runner acc n l) {
		if (<= n 0) acc else {
			lappend acc (car l)
			take-runner acc (- n 1) (cdr l)
		}
	}
	take-runner () n l
}
proc (drop n l) {
	if (<= n 0) l else (drop (- n 1) (cdr l))
}
proc (split n l) {
 	list (take n l) (drop n l)
}
proc (match e l) {
	proc (match-runner acc n e l)  {
		if (eq l ()) acc else {
			if (eq (car l) e) (lappend acc n)
			match-runner acc (+ n 1) e (cdr l)
		}
	}
	match-runner () 0 e l
}
proc (elem x l) {
		if (eq 0 (llength (match x l))) false else true
}
proc (reverse l) {
	proc (reverse-runner l acc) {
		if (eq l ()) acc else (reverse-runner (cdr l) (lappend (head l) acc))
	}
	reverse-runner l ()
}
proc (zip l1 l2) {
	proc (zip-worker l1 l2) {
		if (or (eq () l1) (eq () l2)) () else {
			lappend (lappend (head l1) (head l2)) (zip-worker  (cdr l1) (cdr l2))
		}
	}
	split (llength l1) (zip-worker l1 l2)
}

# higher order operators
proc (map f l) {
	proc (map-runner acc f l) {
		if (eq l ()) acc else {
			lappend acc (f (car l))
			map-runner acc f (cdr l)
		}
	}
	map-runner () f l
}
proc (map2 f l1 l2) {
	proc (map2-runner acc f l1 l2) {
		if (or (eq l1 ()) (eq l2 ())) acc else {
			lappend acc (f (car l1) (car l2))
			map2-runner acc f (cdr l1) (cdr l2)
		}
	}
	map2-runner () f l1 l2
}
proc (filter f l) {
	proc (filter-runner acc f l) {
		if (eq l ()) acc else {
			lappend acc (if (f (car l)) (head l))
			filter-runner acc f (cdr l)
		}
	}
	filter-runner () f l
}
proc (unpack f l) (eval (lappend () f l))
proc (foldl f z l) {
 	if (eq () l) z else (foldl f (f z (car l)) (cdr l))
}
proc (flip f a b) (f b a)
proc (comp f g x) (f (g x))
proc (dup n x) {
	proc (dup-runner acc n x) {
		if (<= n 0) acc else {
			lappend acc x
			dup-runner acc (- n 1) x
		}
	}
	dup-runner () n x
}

# miscellaneous
proc (test x y)(if (eq (lappend (list) (eval x)) y) (puts x  " passed\n") else (throw "*** FAILED ***" x))

# arithmetics
proc (succ x) (+ x 1)
proc (pred x) (- x 1)
proc (quotient a b) (floor (/ a b))
proc (remainder a b) (floor (- a (* b (quotient a b))))
proc (mod a b) (floor (- a (* b (quotient a b))))
proc (twice x) (+ x x)
proc (square x) (* x x)
proc (mul-neg) (comp neg (unpack *))
proc (round x) (floor (+ x 0.5))
proc (fac x) {
	proc (fact-iter a product) {
		if (eq a 0) product else (fact-iter (- a 1) (* product a))
	}
	fact-iter x 1
}
proc (fib n) {
	proc (fib-worker a b count) {
		if (<= count 1) b else (fib-worker (+ a b) a (- count 1))
	}
	fib-worker 1 1 n
}
proc (ack m n) {
	if (eq m 0) (+ n 1) else {
		if (eq n 0)	(ack (- m 1) 1) else {
			ack (- m 1) (ack m (- n 1))
		}
	}			
}
proc (birandn n m)(array (map (\ (x)(* x n)) (array2list (noise m))))
proc (randn n m)(array (map (\ (x) (/ (+ n x) 2)) (array2list (birandn n m))))

# list-based operators
proc (zeros n)(bpf 0 n 0)
proc (ones n)(bpf 1 n 1)
proc (diff l) {
	proc(diff-runner acc l) {
		if (eq (cdr l) ()) acc else {
			lappend acc (- (car (cdr l)) (car l))
			diff-runner acc (cdr l)
		}
	}
	diff-runner () l
}
proc (sign l) {
	proc (sign-runner acc l) {
		if (eq l ()) acc else {
			if (>= (car l) 0) (lappend acc 1) else (lappend acc -1)
			sign-runner acc (cdr l)
		}
	}
	sign-runner () l
}
proc (mean l) {
	/ (sum l) (size l)
}
proc (stddev l) {
	set mu (mean l)
	set normal (/ 1. (- (size l) 1))
	# sqrt (* (sum (map square (map (\ (x)(- x mu)) l))) normal)
	sqrt (* (sum (square (- l mu))) normal)
}
proc (dot a b) {
	sum (* a b)
}
proc (ortho a b)(eq (dot a b) 0)
proc (standard l) {
	set mu (mean l)
	set s (stddev l)
	# map (\ (x)(/ x s)) (map (\ (x)(-  x mu)) l)
	/ (-  l mu) s
}
proc (compare op l) {	
	proc (cmp-runner n l) {
		if (eq l ()) n else {
			if (op (car l) n) {
				! n (car l)
				cmp-runner n (cdr l)
			} else (cmp-runner n (cdr l))
		}
	}
	cmp-runner (car l) l
}
proc (normal l) {
	/ l (max l)
}
proc (. f l1 l2) {
	map2 (\ (x y) (f x y)) l1 l2

}
# constants
set TWOPI 6.2831853072
set SQRT2 1.4142135624
set LOG2 0.3010299957
set E 2.7182818284

# system
proc (fexists fname) {
	eq (car (filestat fname)) 1
}
proc (readlines stream) {	
	set line ""
	proc (readlines-runner stream line) {
		! line (tostr line (readline stream))
		if (not (isgood stream)) line else {
			readlines-runner stream line
		}
	}
	readlines-runner stream line
}
proc (udpmonitor addr port) {
	set r (udprecv addr port)
	if (eq r "disconnect") 1 else {
		puts r nl
		udpmonitor addr port
	}
}

# plotting
set Aqua (array '(0 255 255))
set Black (array '(0 0 0))
set Blue (array '(0 0 255))
set Brown (array '(165 42 42))
set Cyan (array '(0 255 255))
set Fuchsia (array '(255 0 255))
set Green (array '(0 128 0))
set Lime (array '(0 255 0))
set Magenta (array '(255 0 255))
set Orange (array '(255 165 0))
set Purple (array '(128 0 128))
set Red (array '(255 0 0))
set Silver (array '(192 192 192))
set White (array '(255 255 255))
set Yellow (array '(255 255 0))
set Gray (array '(127 127 127))
proc (plot name x) {
	set a (opensvg name 512 512)
	polyline a x Blue
	closesvg a
}
proc (scatplot name x y) {
	set a (opensvg name 512 512)
	scatter a x y Blue
	closesvg a
}

# signals
proc (oscbank sr amps freqs tab) {
    # assumes both freqs and amps have the same number of elems
	set elems (llength amps) 
	set outbuff (bpf 0 (size (car freqs)) 0)
	set i 0
	while (< i elems) {
		set f (car freqs)
		if (eq f ()) {break}
		! outbuff (+ outbuff (* (car amps) (osc sr (car freqs) tab)))
		set amps (cdr amps)
		set freqs (cdr freqs)
		! i (+ i 1)
	}
	+ outbuff
}

# ;; eof

