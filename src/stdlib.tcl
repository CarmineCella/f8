# f8 - standard library
#

# macros
macro (let b) (\ () (eval b))

# logical operators
proc (not x) (if x false else true)
proc (and x y)(if (if x true else false) (if y true else false) else false)
proc (or x y) (if x x else (if y y else false))
proc (<> n1 n2) (not (eq n1 n2))

# lists
proc (first l) (lget 0 l)
proc (second l) (lget 0 (ltail l))
proc (third l) (lget 0 (ltail (ltail l)))
proc (len l) {
	proc (len_runner i l) {
		if (eq l ()) i	else (len_runner (+ i 1) (ltail l))
	}
	len_runner 0 l
}
proc (last l) (lget (- (len l) 1) l)
proc (take n l) {
	proc (take_runner acc n l) {
		if (<= n 0) acc else {
			ljoin acc (first l)
			take_runner acc (- n 1) (ltail l)
		}
	}
	take_runner () n l
}
proc (drop n l) {
	if (<= n 0) l else (drop (- n 1) (ltail l))
}
proc (split n l) {
 	list (take n l) (drop n l)
}
proc (match e l) {
	proc (match_runner acc n e l)  {
		if (eq l ()) acc else {
			if (eq (first l) e) (ljoin acc n)
			match_runner acc (+ n 1) e (ltail l)
		}
	}
	match_runner () 0 e l
}
proc (elem x l) {
		if (eq 0 (len (match x l))) false else true
}
proc (reverse l) {
	proc (reverse-runner l acc) {
		if (eq l ()) acc else (reverse-runner (ltail l) (ljoin (lhead l) acc))
	}
	reverse-runner l ()
}

# higher order operators
proc (map f l) {
	proc (map-runner acc f l) {
		if (eq l ()) acc else {
			ljoin acc (f (first l))
			map-runner acc f (ltail l)
		}
	}
	map-runner () f l
}
proc (filter f l) {
	proc (filter-runner acc f l) {
		if (eq l ()) acc else {
			ljoin acc (if (f (first l)) (lhead l))
			filter-runner acc f (ltail l)
		}
	}
	filter-runner () f l
}
proc (unpack f l) (eval (ljoin () f l))
proc (foldl f z l) {
 	if (eq () l) z else (foldl f (f z (first l)) (ltail l))
}
proc  (flip f a b) (f b a)
proc (comp f g x) (f (g x))
proc (dup n x) {
	proc (dup_runner acc n x) {
		if (<= n 0) acc else {
			ljoin acc x
			dup_runner acc (- n 1) x
		}
	}
	dup_runner () n x
}

# miscellaneous
proc (case x) {
	if (== & ()) {
   		throw "case not found" x
    	if (== x (first (first &)))	{
			second (first &)
		} else {
			unpack case (ljoin (coll) x (ltail &))
		}
	}
}
proc (test x y)(if (eq (ljoin (list) (eval x)) y) (puts x  " passed" nl) else (throw "*** FAILED ***" x))

# arithmetics
proc (succ x) (+ x 1)
proc  (pred x) (- x 1)
proc (quotient a b) (floor (/ a b))
proc (remainder a b) (floor (- a (* b (quotient a b))))
proc (mod a b) (floor (- a (* b (quotient a b))))
proc (twice x) (+ x x)
proc (square x) (* x x)
proc (mul-neg) (comp - (unpack *))
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
proc (birandn n m)(map (\ (x)(* x n)) (noise m))
proc (randn n m)(map (\ (x) (/ (+ n x) 2)) (birandn n m))

# list-based operators
#(def (zeros)(flip dup 0))
#(def (ones)(flip dup 1))
proc (zeros n)(dup n 0)
proc (ones n)(dup n 1)
proc (diff l) {
	proc(diff_runner acc l) {
		if (eq (ltail l) ()) acc else {
			ljoin acc (- (first (ltail l)) (first l))
			diff_runner acc (ltail l)
		}
	}
	diff_runner () l
}
proc (sign l) {
	proc (sign_runner acc l) {
		if (eq l ()) acc else {
			if (>= (first l) 0) (ljoin acc 1) else (ljoin acc -1)
			sign_runner acc (ltail l)
		}
	}
	sign_runner () l
}
proc (sum l)(foldl + 0 l)
proc (prod l)(foldl * 1 l)
proc (mean l)(/ (foldl + 0 l) (len l))
proc (stddev l) {
	set mu (mean l)
	set normal (/ 1. (- (len l) 1))
	sqrt (* (sum (map square (map (\ (x)(- x mu)) l))) normal)
}
proc (dot a b) {
	if (or (eq a ()) (eq b ())) 0 else {
		+ (* (first a) (first b)) (dot (ltail a) (ltail b))
	}
}
proc (ortho a b)(eq (dot a b) 0)
proc (standard l) {
	set mu (mean l)
	set s (stddev l)
	map (\ (x)(/ x s)) (map (\ (x)(-  x mu)) l)
}
proc (compare op l) {	
	proc (cmprunner n l) {
		if (eq l ()) n else {
			if (op (first l) n) {
				! n (first l)
				cmprunner n (ltail l)
			} else (cmprunner n (ltail l))
		}
	}
	cmprunner (first l) l
}
proc (min l) ((compare <) l)
proc (max l) ((compare >) l)
proc (normal l) {
	set m (max l)
	map (\ (x)(/ x m)) l
}

# constants
set TWOPI 6.2831853072
set SQRT2 1.4142135624
set LOG2 0.3010299957
set E 2.7182818284

# system
proc (fexists fname) {
	eq (first (filestat fname)) 1
}
proc (udpmonitor addr port) {
	set r (udprecv addr port)
	if (eq r "disconnect") 0 else {
		puts r nl
		udpmonitor addr port
	}
}

# plotting
set Aqua '(0 255 255)
set Black '(0 0 0)
set Blue '(0 0 255)
set Brown '(165 42 42)
set Cyan '(0 255 255)
set Fuchsia '(255 0 255)
set Green '(0 128 0)
set Lime '(0 255 0)
set Magenta '(255 0 255)
set Orange '(255 165 0)
set Purple '(128 0 128)
set Red '(255 0 0)
set Silver '(192 192 192)
set White '(255 255 255)
set Yellow '(255 255 0)
set Gray '(127 127 127)
proc (plot name x) {
	set a (opensvg name 512 512)
	polyline a x Blue
	closesvg a
}

# ;; eof

