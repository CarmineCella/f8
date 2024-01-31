;; Some minimal tests

(begin
  	(load "bootstrap.scm")
   
	(begin (define x 1) (set! x (+ x 1)) (display (+ x 1)) (newline))
	(display (+ 1 2)) (newline)
	(define a (+ 4 1))
	(define b a)
	(define double (lambda (x) (+ x x)))
	(display (double 5)) (newline)
	(display (+ (double b) a a)) (newline)
	(display ((lambda (x) (+ 4 x)) 5)) (newline)

	(define i 0)           
	(do ((i 0 (+ i 1))) ((= i 5) i) (display i) (spaces 1))
	(display (case (* 2 3) ((2 3 5 7) 'prime) ((1 4 6 8 9) 'composite))) (newline) 
)

;; eof