;; f8 - standard library
;;

;; logical operators
(def (not x) (if x #f #t))
(def (and x y)(if (if x #t #f) (if y #t #f) #f))
(def (or x y) (if x x (if y y #f)))
(def (<> n1 n2) (not (= n1 n2)))

;; lists
(def (first l) (lget 0 l))
(def (second l) (lget 0 (ltail l)))
(def (third l) (lget 0 (ltail (ltail l))))
(def (len l) {
	(def (len_runner i l)
		(if (eq l ()) i	(len_runner (+ i 1) (ltail l))))
	(len_runner 0 l)
})
(def (last l) (lget (- (len l) 1) l))
(def (take n l) {
	(def (take_runner acc n l) 
		(if (<= n 0) acc {
			(ljoin acc (first l))
			(take_runner acc (- n 1) (ltail l))
		}))
	(take_runner () n l)
})
(def (drop n l) 
	(if (<= n 0) l (drop (- n 1) (ltail l)))
)
(def (split n l) 
	(list (take n l) (drop n l))
)
(def (match e l) {
	(def (match_runner acc n e l) 
		(if (eq l ()) acc {
			(if (eq (first l) e) (ljoin acc n))
			(match_runner acc (+ n 1) e (ltail l))
		}))
	(match_runner () 0 e l)
})
(def (elem x l) (
	if (eq 0 (len (match x l))) #f #t)
)
(def (reverse l) {
	(def (reverse-runner l acc)
		(if (eq l ()) acc (reverse-runner (ltail l) (ljoin (lhead l) acc))))
	(reverse-runner l ())
})

;; higher order operators
(def (map f l) {
	(def (map-runner acc f l)
		(if (eq l ()) acc {
			(ljoin acc (f (first l)))
			(map-runner acc f (ltail l))
		}))
	(map-runner () f l)
})
(def (filter f l) {
	(def (filter-runner acc f l)
		(if (eq l ()) acc {
			(ljoin acc (if (f (first l)) (lhead l)))
			(filter-runner acc f (ltail l))
		})	
	)
	(filter-runner () f l)
})
(def (unpack f l) (eval (ljoin () f l)))
(def (foldl f z l)
	(if (eq () l) z (foldl f (f z (first l)) (ltail l)))
)
(def (flip f a b) (f b a))
(def (comp f g x) (f (g x)))
(def (dup n x) {
	(def (dup_runner acc n x) 
		(if (<= n 0) acc {
			(ljoin acc x)
			(dup_runner acc (- n 1) x)
		}))
	(dup_runner () n x)
})

;; miscellaneous
(def (case x) 
	(if (== & ())
   		(throw "case not found" x)
    	(if (== x (first (first &))) 
    		(second (first &)) 
    		(unpack case (ljoin (coll) x (ltail &)))
    	)
   )
)
(def (test x y)(if (eq (ljoin (list) (eval x)) y) (do (print x  " passed" (nl)) )(throw "*** FAILED ***" x)))

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
(def (fac x) {
	(def (fact-iter a product)
		(if (eq a 0) product (fact-iter (- a 1) (* product a))))
	(fact-iter x 1)
})
(def (fib n) {
	(def (fib-worker a b count)
		(if (<= count 1) b (fib-worker (+ a b) a (- count 1))))
	(fib-worker 1 1 n)
})
(def (ack m n)
	(if (eq m 0)
		(+ n 1)
		(if (eq n 0)
			(ack (- m 1) 1)
			(ack (- m 1) (ack m (- n 1))))))
(def (birandn n m)(map (\ (x)(* x n)) (noise m)))
(def (randn n m)(map (\ (x) (/ (+ n x) 2)) (birandn n m)))

;; list-based operators
;(def (zeros)(flip dup 0))
;(def (ones)(flip dup 1))
(def (zeros n)(dup n 0))
(def (ones n)(dup n 1))
(def (diff l) {
	(def (diff_runner acc l) 
		(if (eq (ltail l) ()) acc {
			(ljoin acc (- (first (ltail l)) (first l)))
			(diff_runner acc (ltail l))
		}))
	(diff_runner () l)
})
(def (sign l) {
	(def (sign_runner acc l) 
		(if (eq l ()) acc {
			(if (>= (first l) 0) (ljoin acc 1) (ljoin acc -1))
			(sign_runner acc (ltail l))
		}))
	(sign_runner () l)
})
(def (sum l)(foldl + 0 l))
(def (prod l)(foldl * 1 l))
(def (mean l)(/ (foldl + 0 l) (len l)))
(def (stddev l) {
	(def mu (mean l))
	(def normal (/ 1. (- (len l) 1)))
	(sqrt (* (sum (map square (map (\ (x)(- x mu)) l))) normal))
})
(def (dot a b)
	(if (or (eq a ()) (eq b ())) 0
		(+ (* (first a) (first b)) (dot (ltail a) (ltail b)))))
(def (ortho a b)(eq (dot a b) 0))
(def (standard l) {
	(def mu (mean l))
	(def s (stddev l))
	(map (\ (x)(/ x s)) (map (\ (x)(-  x mu)) l))
})
(def (compare op l) {	
	(def (cmprunner n l)
		(if (eq l ())
			n
			(if (op (first l) n) {
				(set! n (first l)) (cmprunner n (ltail l))
			} (cmprunner n (ltail l)))))
	(cmprunner (first l) l)
})
(def (min l) ((compare <) l))
(def (max l) ((compare >) l))
(def (normal l) {
	(def m (max l))
	(map (\ (x)(/ x m)) l)
})

 ;; constants
(def TWOPI 6.2831853072)
(def SQRT2 1.4142135624)
(def LOG2 0.3010299957)
(def E 2.7182818284)

;; system
(def (exists fname)
	(eq (first (filestat fname)) 1))
(def (udpmonitor addr port) {
	(def (r) (udprecv addr port))
	(if (== r "disconnect") 0 {
		(prn r (nl))
		(udpmonitor addr port)
	})
})

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
(def (plot name x) {
	(def a (opensvg name 512 512))
	(polyline a x Blue)
	(closesvg a)
})

;; eof

