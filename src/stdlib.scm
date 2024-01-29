;; f8 - standard library
;;

;; logical operators
(def (not x) (if x #f #t))
(def (and x y)(if (if x #t #f) (if y #t #f) #f))
(def (or x y) (if x x (if y y #f)))
(def (<> n1 n2) (not (= n1 n2)))

;; lists
(def (first l) (nth 0 l))
(def (second l) (nth 0 (tail l)))
(def (third l) (nth 0 (tail (tail l))))
(def (len l) 
	(do
		(def (len_runner i l)
			(if (eq l ()) 
				i
				(len_runner (+ i 1) (tail l)))
			)
		(len_runner 0 l)
	)
)
(def (last l) (nth (- (len l) 1) l))
(def (take n l) 
	(do
		(def (take_runner acc n l) 
			(if (<= n 0)
				acc
				(do
					(join acc (first l))
					(take_runner acc (- n 1) (tail l))
				)
			)	
		)
		(take_runner () n l)
	)
)
(def (drop n l) (
	if (<= n 0)
		l
		(drop (- n 1) (tail l))
	)
)
(def (split n l) 
	(list (take n l) (drop n l))
)
(def (match e l) 
	(do
		(def (match_runner acc n e l) 
			(if (eq l ())
				acc
				(do
					(if (eq (first l) e) (join acc n))
					(match_runner acc (+ n 1) e (tail l))
				)
			)	
		)
		(match_runner () 0 e l)
	)
)
(def (elem x l) (
	if (eq 0 (len (match x l)))
		#f
		#t
	)
)
(def (reverse l)
	(do
		(def (reverse-runner l acc)
			(if (eq l ())
  				acc
      			(reverse-runner (tail l) (join (head l) acc))
			)	
		)
	(reverse-runner l '(0)))
)

;; higher order operators
(def (map f l)
	(do
		(def (map-runner acc f l)
			(if (eq l ())
  				acc
				(do
					(join acc (f (first l)))
	      			(map-runner acc f (tail l))
				)
			)	
		)
		(map-runner () f l)
	)
)
(def (filter f l)
	(do
		(def (filter-runner acc f l)
			(if (eq l ())
  				acc
				(do
					(join acc (if (f (first l)) (head l)))
	      			(filter-runner acc f (tail l))
				)
			)	
		)
		(filter-runner () f l)
	)
)
(def (unpack f l) (eval (join () f l)))
(def (foldl f z l)(
	if (eq () l)
		z
		(foldl f (f z (first l)) (tail l))
	)
)
(def (flip f a b) (f b a))
(def (comp f g x) (f (g x)))
(def (dup n x) 
	(do
		(def (dup_runner acc n x) 
			(if (<= n 0)
				acc
				(do
					(join acc x)
					(dup_runner acc (- n 1) x)
				)
			)	
		)
		(dup_runner () n x)
	)
)

;; miscellaneous
(def (case x) (
 	if (== & ())
   		(throw "case not found" x)
    	(if (== x (first (first &))) 
    		(second (first &)) 
    		(unpack case (join (coll) x (tail &)))
    	)
   )
)
(def (test x y)(
	if (eq (join (list) (eval x)) y) (do (print x  " passed" (nl)) )(throw "*** FAILED ***" x)
	)	
)

;; arithmetics
(def (succ x) (+ x 1))
(def (pred x) (- x 1))
(def (quotient a b) (floor (/ a b)))
(def (remainder a b) (floor (- a (* b (quotient a b)))))
(def (mod a b) (floor (- a (* b (quotient a b)))))
(def (twice x) (+ x x))
(def (square x) (* x x))
(def (mul-neg) (comp - (unpack *)))
(def (round x) (floor (+ x 0.5)))
(def (fac x)
	(do
		(def (fact-iter a product)
			(if (eq a 0)
				product
				(fact-iter (- a 1) (* product a))
			)
		)
		(fact-iter x 1)
	)
)
(def (fib n)
	(do 
		(def (fib-worker a b count)
			(if (<= count 1)
				b
				(fib-worker (+ a b) a (- count 1))
			)
		)
		(fib-worker 1 1 n)
	)
)
(def (ack m n)
	(if (eq m 0)
		(+ n 1)
		(if (eq n 0)
			(ack (- m 1) 1)
			(ack (- m 1) (ack m (- n 1)))
		)
	)
)
(def (birandn n m)(map (lambda (x)(* x n)) (noise m)))
(def (randn n m)(map (lambda (x) (/ (+ n x) 2)) (birandn n m)))

;; list-based operators
;(def (zeros)(flip dup 0))
;(def (ones)(flip dup 1))
(def (zeros n)(dup n 0))
(def (ones n)(dup n 1))
(def (diff l) 
	(do
		(def (diff_runner acc l) 
			(if (eq (tail l) ())
				acc
				(do
					(join acc (- (first (tail l)) (first l)))
					(diff_runner acc (tail l))
				)
			)	
		)
		(diff_runner () l)
	)
)
(def (sign l) 
	(do
		(def (sign_runner acc l) 
			(if (eq l ())
				acc
				(do
					(if (>= (first l) 0) (join acc 1) (join acc -1))
					(sign_runner acc (tail l))
				)
			)	
		)
		(sign_runner () l)
	)
)
(def (sum l)(foldl + 0 l))
(def (prod l)(foldl * 1 l))
(def (mean l)(/ (foldl + 0 l) (len l)))
(def (stddev l)
	(do
		(def mu (mean l))
		(def normal (/ 1. (- (len l) 1)))
		(sqrt (* (sum (map square (map (lambda (x)(- x mu)) l))) normal))
	)
)
(def (dot a b)
	(if (or (eq a ()) (eq b ()))
		0
		(+ (* (first a) (first b)) (dot (tail a) (tail b)))
	)
)
(def (ortho a b)(eq (dot a b) 0))
(def (standard l)
	(do
		(def mu (mean l))
		(def s (stddev l))
		(map (lambda (x)(/ x s)) (map (lambda (x)(-  x mu)) l))
	)
)
(def (compare op l)
	(do	
		(def (cmprunner n l)
			(if (eq l ())
				n
				(if (op (first l) n)
					(do (set! n (first l)) (cmprunner n (tail l)))
					(cmprunner n (tail l))			
				)
			)
		)
		(cmprunner (first l) l)
	)
)
(def (min l) ((compare <) l))
(def (max l) ((compare >) l))
(def (normal l)
	(do 
		(def m (max l))
		(map (lambda (x)(/ x m)) l)
	)
)

 ;; constants
(def TWOPI 6.2831853072)
(def SQRT2 1.4142135624)
(def LOG2 0.3010299957)
(def E 2.7182818284)

;; system
(def (exists fname)
	(eq (first (filestat fname)) 1)
)
(def (udpmonitor addr port)
	(do
	 	(def (r) (udprecv addr port))
		(if (== r "disconnect")
			(do (join 0))
			(do	(prn r (nl))
				(udpmonitor addr port)
			)
		)
	)
)

;; plotting
(def Aqua '(0 255 255))
(def Black '(0 0 0))
(def Blue '(0 0 255))
(def Brown '(165 42 42))
(def Cyan '(0 255 255))
(def Fuchsia '(255 0 255))
(def Green '(0 128 0))
(def Lime '(0 255 0))
(def Magenta '(255 0 255))
(def Orange '(255 165 0))
(def Purple '(128 0 128))
(def Red '(255 0 0))
(def Silver '(192 192 192))
(def White '(255 255 255))
(def Yellow '(255 255 0))
(def Gray '(127 127 127))
(def (plot name x) (
	do
	(def a (opensvg name 512 512))
	(polyline a x Blue)
	(closesvg a)
))

;; eof

