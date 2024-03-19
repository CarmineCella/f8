# --------------------------------
# The f8 (fate) scripting language
# --------------------------------
#
# Standard library
#
# (c) www.carminecella.com
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
proc (cdr l) (lrange l 1 (- (llength l) 1))
proc (second l) (lindex l 1)
proc (third l) (lindex l 2)
proc (lhead l) (lappend () (car l))
proc (llast l) (lindex l (- (llength l) 1))
proc (ltail l) (lappend () (llast l))
proc (ltake l n) {
	proc (ltake-runner acc l n) {
		if (<= n 0) acc else {
			if (not (eq (car l) ())) (lappend acc (car l))
			ltake-runner acc (cdr l) (- n 1)
		}
	}
	ltake-runner () l n
}
proc (ldrop l n) {
	if (<= n 0) l else (ldrop (cdr l)(- n 1))
}
proc (lsplit l n) {
 	list (ltake l n) (ldrop l n) 
}
proc (lreverse l) {
	proc (lreverse-runner l acc) {
		if (eq l ()) acc else (lreverse-runner (cdr l) (lappend () (car l) acc))
	}
	lreverse-runner l ()
}
proc (lreverse l) {
    set res ()
    set i (- (llength l) 1)
    while (>= i 0) {
		lappend res (lindex l i)
		= i (- i 1)
	}
    set res res
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
proc (zip l1 l2) {
	map2 list l1 l2
}
proc (dup n x) {
	proc (dup-runner acc n x) {
		if (<= n 0) acc else {
			lappend acc x
			dup-runner acc (- n 1) x
		}
	}
	dup-runner () n x
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
			if (f (car l)) (lappend acc (car l))
			filter-runner acc f (cdr l)
		}
	}
	filter-runner () f l
}
proc (foldl f z l) {
 	if (eq () l) z else (foldl f (f z (car l)) (cdr l))
}
proc (flip f a b) (f b a)
proc (comp f g x) (f (g x))

# arithmetics
proc (getval x n) (slice x n 1)
proc (setval x v n) (assign x v n 1)
proc (succ x) (+ x 1)
proc (pred x) (- x 1)
proc (quotient a b) (floor (/ a b))
proc (remainder a b) (floor (- a (* b (quotient a b))))
proc (mod a b) (floor (- a (* b (quotient a b))))
proc (twice x) (+ x x)
proc (square x) (* x x)
proc (round x) (floor (+ x 0.5))
proc (birandn n m)(* (noise m) n)
proc (randn n m)(/ (+ (birandn n m) n) 2)
proc (zeros n)(bpf 0 n 0)
proc (ones n)(bpf 1 n 1)
proc (mean l) {
	/ (sum l) (size l)
}
proc (stddev x) {
	set mu (mean x)
	set normal (/ 1. (- (size x) 1))
	# sqrt (* (sum (map square (map (\ (x)(- x mu)) l))) normal)
	sqrt (* (sum (square (- x mu))) normal)
}
proc (standard x) {
	set mu (mean x)
	set s (stddev x)
	# map (\ (x)(/ x s)) (map (\ (x)(-  x mu)) l)
	/ (-  x mu) s
}
proc (normal x) {
	/ x (max x)
}
proc (dot a b) {
	sum (* a b)
}
proc (ortho a b)(eq (dot a b) 0)
proc (diff x)(- (slice x 1 (size x)) x)
proc (fac x) {
	proc (fact-worker a product) {
		if (eq a 0) product else (fact-worker (- a 1) (* product a))
	}
	fact-worker x 1
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
proc (deinterleave in) {
	set sz (size in)
    set out1 (slice in 0 (/ sz 2) 2)
    set out2  (slice in 1 (/ sz 2) 2)	
	list out1 out2
}
proc (interleave in) {
	set in1 (car in)
	set in2 (second in)
    set out (bpf 0 (+ (size in1) (size in2)) 0)
    assign out in1 0 (size in1) 2
    assign out in2 1 (size in2) 2
}

# list-based operators
proc (sign l) {
	proc (sign-runner acc l) {
		if (eq l ()) acc else {
			if (>= (car l) 0) (lappend acc 1) else (lappend acc -1)
			sign-runner acc (cdr l)
		}
	}
	sign-runner () l
}
proc (compare op l) {	
	proc (cmp-runner n l) {
		if (eq l ()) n else {
			if (op (car l) n) {
				= n (car l)
				cmp-runner n (cdr l)
			} else (cmp-runner n (cdr l))
		}
	}
	cmp-runner (car l) l
}
proc (. f l1 l2) {
	map2 (\ (x y) (f x y)) l1 l2
}

# constants
set TWOPI 6.2831853072
set SQRT2 1.4142135624
set LOG2 0.3010299957
set E 2.7182818284

# eof


