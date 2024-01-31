
;; guess.scm

(begin
	(load "bootstrap.scm") ; for random numbers
 	(set! *seed* (system-ticks))
 
   	(define guess (lambda (x)
      (display "number: ")
      (define v (read))
        
    	(if (= x v)
     		(display "you won, bravo!\n\n")
       		(begin
       			(if (> x v)
            	 	(display "too low!\n")
              	(display "too high!\n"))
         		(guess x)))))
    
 	(display "Guess a number!!!\n\n") 
 	(define val (modulo (random-next) 100))
 	(guess val))

;; eof

