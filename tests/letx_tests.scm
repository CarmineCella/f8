;; Some tests for let, letrec, let* forms
;;

(begin
	(load "bootstrap.scm")
	
	(define (zero? x) (if (= x 0) #t #f))
	(define (sub1 x) (- x 1))
	(define (not x) (if x #f #t))
	
	(display (let ((x 5)) x)) (newline)
	(display (let ((x 5))
			   (let ((x 2)
					 (y x))
				 (list y x)))) (newline)
                     
	(display (let* ((x 1) (y x)) (+ x y))) (newline)        
                             
	(display (let facx ((n 10)) (if (zero? n) 1 (* n (facx (sub1 n)))))) (newline)
										   
   	(display (let loop ((numbers '(3 -2 1 6 -5))
   	               (nonneg '())
   	               (neg '()))
   	      (cond ((null? numbers) (list nonneg neg))
   	            ((>= (car numbers) 0)
   	             (loop (cdr numbers)
   	                   (cons (car numbers) nonneg)
   	                   neg))
   	            ((< (car numbers) 0)
   	             (loop (cdr numbers)
   	                   nonneg
   	(cons (car numbers) neg)))))) (newline)
				
	(display (letrec ((sum (lambda (x) (if (zero? x) 0 (+ x (sum (- x 1))))))) (sum 5))) (newline)

	(display (letrec ((even? (lambda (n) (if (zero? n) #t (odd? (- n 1))))) (odd? (lambda (n) (if (zero? n) #f (even? (- n 1)))))) (even? 88))) (newline)
	
	(display (let ((delayed (delay (+ 5 6)))) (force delayed))) (newline)
	(letrec ((countdown (lambda (i) (if (= i 0) 'liftoff (begin (display i) (newline) (countdown (- i 1))))))) (countdown 10))
	(let countdown ((i 10)) (if (= i 0) 'liftoff (begin (display i) (newline) (countdown (- i 1)))))
	
	(display (let ()
		(define even? (lambda (n) (if (zero? n) #t (odd? (- n 1)))))
		(define odd? (lambda (n) (if (zero? n) #f (even? (- n 1)))))
	(even? 88))) (newline)
	
	
	(display (let ((frumble "froz"))
	   (define banana (lambda () (apple 'peach)))
	   (define apple (lambda (x) x))
	   (banana))) (newline)
)

;; eof