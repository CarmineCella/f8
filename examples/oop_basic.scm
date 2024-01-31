;; Basic example of OOP in Vuza (not using the objects library)
;;

(begin
	(load "bootstrap.scm")

	;; simple action-based reaction, with default behaviour
	(define (animal action)
		(cond ((eq? action 'speak) nil)
			  ((eq? action 'move) (begin (display "walk walk...") (newline)))
			  (else (error "undefined action" action))))
	
    (define (dog action)
		(cond ((eq? action 'speak) (begin (display "ouf oof!") (newline)))
			  (else (animal action))))
			  
    (define (cat action)
		(cond ((eq? action 'speak) (begin (display "meow...") (newline)))
			  (else (animal action))))			  

  	(define (snake action)
		(cond ((eq? action 'move) (begin (display "...sneak...") (newline)))
		 	 (else (animal action))))			  
			 
	(animal 'move)
	(dog 'speak)		 
	(dog 'move)
	(cat 'speak)
	(snake 'speak)
	(snake 'move)
	(newline)

	;; creation and usage of a simple object, with getter and setter
	(define (make-simple-object) 
	   (let ((var 0)) 
	     (define init (lambda () (set! var 0) var)) 
	     (define var-set! (lambda (x) (set! var x) var)) 
	     (define var-get (lambda () var)) 
	     (define var-display 
	       (lambda () 
	         (newline) 
	         (display "var = ") 
			 (display var))) 
			 
	     (lambda (message . args) 
	       (apply (case message 
	                  ((init) init) 
	                  ((var-set!) var-set!) 
	                  ((var-display) var-display) 
	                  ((var-get) var-get) 
	                  (else (error "unknown method" message))) 
	              args)))) 
  
	 (define a (make-simple-object))  
	 (define b (make-simple-object)) 	 

	 (a 'init)
	 (b 'init)
	 (display (a 'var-set! 4)) (newline)
	 (display (a 'var-get)) (newline) 
	 (display (b 'var-get)) (newline) 
	 (a 'var-display)
	 (b 'var-display)
	 (newline)
	 
	 (a 'fake-method) ;; raise error!
)

;; eof

