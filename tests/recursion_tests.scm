;; {tests for evlis recursion}
;;
;; (c) 2019 www.arch-programming.org
;;

(load "core.scm")

(def {t0} (\{x}{let {do (def {hh} 44) (+ hh x)}}))

(function {t1 x}{
	do
	(let {do
		(function {t2 y}{
			+ x y
		})
		(t2 3)})
})

(function {p1 v}{ 
	do (function {p2}{
			do (def {g} 3) (-(+ g v))
		})
		(p2 49)
})

(def {t3} (\{v}{
	do (def {t4}(\{}{
			do (def {g} 3) (if (- v v (* 3 v)) {+ g v} {/ 3 2 v})
		}))
		(t4 49)
}))

(def {h13} ((\{v}{(\{}{do (def {g} 3) (+ g v)})}) 10)) ; it works

;;;;;;;;;

(prn "--- evlis recursion ---" (nl) (nl))
(test {t0 1}{45})
(test {t1 1}{4})
(test {p1 1}{-4})
(test {t3 1}{4})
(test {join h13}{13})

(function {rec x}{do (prn x (nl)) (rec (+ 1 x))})
 
(function {trec x}{do (prn x (nl)) (+ 1 (trec x))})
;;(rec 1)  ; infinite recursion - this never stops
;;(trec 1) ; infinite full tail recursion - this crashes

(prn (nl) "ALL TESTS PASSED" (nl) (nl))