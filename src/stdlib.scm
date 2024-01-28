; f8 - standard library
;

;; atoms
(def {true} 1)
(def {false} 0)

;; fundamental operators
(def {function} (@{_f_ _b_} {updef (head _f_) (\ (tail _f_) _b_)}))
(def {dynamic} (@{_f_ _b_} {updef (head _f_) (@ (tail _f_) _b_)}))
(dynamic {let b} { (\ {_} b) {}})

;; coll manipulation

(function {second l} { first (tail l)})
(function {third l} { first (tail (tail l))})

(function {len l} {
	if (== {} l)
		{+ 0}
		{+ 1 (len (tail l))}
	}
)
(function {nth n l} {
	if (<= n 0)
		{head l}
		{nth (- n 1) (tail l)}
	}
)
(function {last l} {nth (- (len l) 1) l})
(function {take n l} {
	if (> n 0)
		{join (head l) (take (- n 1) (tail l))}
	}
)
(function {drop n l} {
	if (<= n 0)
		{join l}
		{drop (- n 1) (tail l)}
	}
)
(function {split n l} 
	{coll (take n l) (drop n l)}
)

(function {match e l} {
	if (== l {})
		{}
		{if (== (join (coll) e) (head l))
			{join l}
			{match e (tail l)}
		}
	}
)
(function {elem x l} {
	if (== 0 (len (match x l)))
		{join false}
		{join true}
	}
)
(function {reverse l} {
	if (== {} l)
		{}
		{join (coll) (reverse (tail l)) (head l)}
	}
)
(function {zip l1 l2}{
	let 
		{do	
			(function {zip-worker l1 l2}
				{if (or (== {} l1) (== {} l2))
					{}
					{join (join (head l1) (head l2)) (zip-worker  (tail l1) (tail l2))}
				}
			)
		(split (len l1) (zip-worker l1 l2))
		}
	}
)

;; higher order operators
(function {map f l} {
	if (== {} l)
		{}
		{join (coll (f (first l))) (map f (tail l))}
	}
)
(function {filter f l} {
	if (== l {})
    {}
    {join (if (f (first l)) {head l} (coll)) (filter f (tail l))}
})

(function {unpack f l} {eval (join {} f l)})
(function {pack f } {f &})
(function {foldl f z l}{
	if (== {} l)
		{join z}
		{foldl f (f z (first l)) (tail l)}
	}
)
(function {flip f a b} {f b a})
(function {comp f g x} {f (g x)})
(function {loop n x}{
	if (> n 0)
		{do (eval x) (loop (- n 1) x)}
	}
)
(function {dup n x}{
	if (> n 0)
		{join (coll) x (dup (- n 1) x)}
	}
)

;; logical operators
(function {not x} {if x {join 0} {join 1}})
(function {or x y}  {if (+ x y) {join 1} {join 0}})
(function {and x y} {if (* x y) {join 1} {join 0}})
(function {<> n1 n2} {not (== n1 n2)})

(function {case x} {
 	if (== & {})
   		{throw "case not found" x}
    	{if (== x (first (first &))) 
    		{second (first &)} 
    		{unpack case (join (coll) x (tail &))}
    	}
   }
)

;; miscellaneous
(function {whos}{
	do 
		(function {iterenv e}
			{if (== {} e)
				{}
				{join (head e) (iterenv (tail (tail e)))}
			}
		)
		(iterenv (tail (first (first (env))))) ; upper environment
	}
)

(function {test x y}{
	if (== (join (coll) (eval x)) y) {do (prn x  " passed" (nl)) }{throw "*** FAILED ***" x}
	}	
)

;; system
(function {exists fname}
	{first (filestat fname)}
)

(function {udpmonitor addr port}
	{do
	 	(def {r} (udprecv addr port))
		(if (== r "disconnect")
			{do (join 0)}
			{do	(prn r (nl))
				(udpmonitor addr port)
			}
		)
	}
)

;; arithmetics
(function {succ x} {+ x 1})
(function {pred x} {- x 1})
(function {quotient a b} {floor (/ a b)})
(function {mod a b} {floor (- a (* b (quotient a b)))})
(function {twice x} {+ x x})
(function {square x} {* x x})
(def {mul-neg} (comp - (unpack *)))

(function {round x} {floor (+ x 0.5)})

;; special functions
(function {fac n} {
	if (<= n 0)
		{first 1}
		{* n (fac (- n 1))}
	}
)

(function {fib n}
	{do 
		(function {fib-worker a b count}
			{if (<= count 1)
				{first b}
				{fib-worker (+ a b) a (- count 1)}
			}
		)
		(fib-worker 1 1 n)
	}
)
(function {ack m n}
	{if (== m 0)
		{+ n 1}
		{if (== n 0)
			{ack (- m 1) 1}
			{ack (- m 1) (ack m (- n 1))}
		}
	}
)
(def {seed} (ticks))
(function {rand}
	{do 
		(def {a} 16807) 
		(def {m} 2147483647) 
		(def {q} (quotient m a)) 
		(def {r} (mod m a))

	  	(! {seed}
			(- (* a (- seed (* (quotient seed q) q)))
				(* (quotient seed q) r)))
	   (if (< seed 0) (! seed (+ seed m)))
	   (+ seed)
	}
)		  
(function {randn n}{mod (rand) n})
(function {birandn n}{- (mod (rand) n) (/ n 2)})
(function {frand}{/ (rand) 2147483647})
(function {frandn n}{* (frand) n})
(function {bifrandn n}{- (* (frand) n) (/ n 2)})

;; coll-based operators
(function {linspace x y step}
	{if (>= x y)
		{}
		{join (coll) x (linspace (+ x step) y step)}
	}
)
(def {zeros}(flip dup 0))
(def {ones}(flip dup 1))
(function {diff l}
	{if (== (tail l) {})
		l
		{join (coll) (- (first (tail l)) (first l)) (diff (tail l))}
	}
)
(function {sign l}
	{if (== l {})
		{}
		{join (coll) (if (>= (first l) 0) {join 1}{join -1}) (sign (tail l))}
	}
)
(function {sum l}{foldl + 0 l})
(function {prod l}{foldl * 1 l})
(function {mean l}{/ (foldl + 0 l) (len l)})
(function {stddev l}
	{do
		(def {mu} (mean l))
		(def {normal} (/ 1. (- (len l) 1)))
		(sqrt (* (sum (map square (map (\{x}{- x mu}) l))) normal))
	}
)
(function {dot a b}
	{if (or (== a {}) (== b {}))
		{}
		{+ (* (first a) (first b)) (dot (tail a) (tail b))}
	}

)
(function {ortho a b}{== (dot a b) 0})
(function {standard l}
	{do
		(def {mu} (mean l))
		(def {s} (stddev l))
		(map (\{x}{/ x s}) (map (\{x}{-  x mu}) l))
	}
)
(function {compare op l}
	{do	
		(function {cmprunner n l}
			{if (== l {})
				{join n}
				{if (op (first l) n)
					{do (! {n} (first l)) (cmprunner n (tail l))}
					{cmprunner n (tail l)}			
				}
			}
		)
		(cmprunner (first l) l)
	}
)
(def {min} (compare <))
(def {max} (compare >))
(function {normal l}
	{do 
		(def {m} (max l))
		(map (\{x}{/ x m}) l)
	}
)
;; dotted operators (builtin remapped to sequences)
(def {.+}(unpack +)) ; equivalent to sum
(def {.*}(unpack *))
(def {.-}(unpack -))
(def {./}(unpack /))
(def {.<}(unpack <))
(def {.<=}(unpack <=))
(def {.>}(unpack >))
(def {.>=}(unpack >=))
(def {.sin}(map sin))
(def {.cos}(map cos))
(def {.sqrt}(map sqrt))
(def {.log}(map log))
(def {.abs}(map abs))
(def {.exp}(map exp))
(def {.floor}(map floor))
(def {.round}(map round))

 ;; constants

 (def {pi} 6.2831853072)
 (def {sqrt2} 1.4142135624)
 (def {log2} 0.3010299957)
 (def {e} 2.7182818284)

(def {Aqua} {0 255 255})
(def {Black} {0 0 0})
(def {Blue} {0 0 255})
(def {Brown} {165 42 42})
(def {Cyan} {0 255 255})
(def {Fuchsia} {255 0 255})
(def {Green} {0 128 0})
(def {Lime} {0 255 0})
(def {Magenta} {255 0 255})
(def {Orange} {255 165 0})
(def {Purple} {128 0 128})
(def {Red} {255 0 0})
(def {Silver} {192 192 192})
(def {White} {255 255 255})
(def {Yellow} {255 255 0})
(def {Gray} {127 127 127})

(function {plot name x} {
	do
	(def {a} (opensvg name 512 512))
	(polyline a x Blue)
	a
})
