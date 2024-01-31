;; Peter Norvig's tests for lispy t
;;

(begin
	(load "bootstrap.scm")
			
	(display (quote (testing 1 (2.0) -3.14e159))) (newline)
	(display (+ 2 2)) (newline)
	(display (+ (* 2 100) (* 1 10))) (newline)
	(display (if (> 6 5) (+ 1 1) (+ 2 2))) (newline)
	(display (if (< 6 5) (+ 1 1) (+ 2 2))) (newline)
	(define x 3)
	(display x) (newline)
	(display (+ x x)) (newline)
	(display (begin (define x 1) (set! x (+ x 1)) (+ x 1))) (newline)
	(display ((lambda (x) (+ x x)) 5)) (newline)
	(define twice (lambda (x) (* 2 x)))
	(display (twice 5)) (newline)
	(define compose (lambda (f g) (lambda (x) (f (g x)))))
	(display ((compose list twice) 5)) (newline)
	(define repeat (lambda (f) (compose f f)))
	(display ((repeat twice) 5)) (newline)
	(display ((repeat (repeat twice)) 5)) (newline)
	(define fact (lambda (n) (if (<= n 1) 1 (* n (fact (- n 1))))))
	(display (fact 3)) (newline)
	(display (fact 12)) (newline)
	(define abs (lambda (n) ((if (> n 0) + -) 0 n)))
	(display (list (abs -3) (abs 0) (abs 3))) (newline)
	(define combine (lambda (f) (lambda (x y) (if (null? x) '() (f (list (car x) (car y)) ((combine f) (cdr x) (cdr y)))) )))
	(define zip (combine cons))
	(display (zip (list 1 2 3 4) (list 5 6 7 8))) (newline)
	(define riff-shuffle (lambda (deck) (begin
		(define take (lambda (n seq) (if (<= n 0) '() (cons (car seq) (take (- n 1) (cdr seq))))))
		(define drop (lambda (n seq) (if (<= n 0) seq (drop (- n 1) (cdr seq)))))
		(define mid (lambda (seq) (/ (- (length seq) 1) 2)))
		((combine append) (take (mid deck) deck) (drop (mid deck) deck)))))
	(display (riff-shuffle (list 1 2 3 4 5 6 7 8))) (newline)
	(display ((repeat riff-shuffle) (list 1 2 3 4 5 6 7 8))) (newline)
	(display (riff-shuffle (riff-shuffle (riff-shuffle (list 1 2 3 4 5 6 7 8)))))  (newline))

;; eof