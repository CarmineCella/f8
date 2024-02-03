;; Environment manipulations

(begin
	(define e0 (make-environment)) ; creates a new empty env
	
	(environment-define e0 'a 32) ; add 'a' to e0
	(environment-define e0 'b 22) ; add 'b' to e0
	(environment-define e0 '+ +) ; add '+' to e0
	
	(display (environment-lookup e0 'a)) (newline) ; should print the value of a
	
	(define foo '(+ a b))
	
	(display (eval foo e0)) (newline) ; should print the sum of a and b
	
	(unbind-variable e0 'b) ; remove 'a' from e0
	
	(display (eval foo e0)) (newline) ; should raise an error
)

;; eof
