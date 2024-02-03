# Basic example of OOP in f8
#

source "stdlib.tcl"

case (> 3 2) 'greater
    (< 3 2) 'less

# simple action-based reaction, with default behaviour
# proc (animal action) {
# 	cond ((eq? action 'speak) ())
# 		(eq? action 'move) (begin (display "walk walk...") (newline)))
# 			else (error "undefined action" action)
# }
# (define (dog action)
# 	(cond ((eq? action 'speak) (begin (display "ouf oof!") (newline)))
# 			(else (animal action))))
			
# (define (cat action)
# 	(cond ((eq? action 'speak) (begin (display "meow...") (newline)))
# 			(else (animal action))))			  

# (define (snake action)
# 	(cond ((eq? action 'move) (begin (display "...sneak...") (newline)))
# 			(else (animal action))))			  
			
# (animal 'move)
# (dog 'speak)		 
# (dog 'move)
# (cat 'speak)
# (snake 'speak)
# (snake 'move)
# (newline)

