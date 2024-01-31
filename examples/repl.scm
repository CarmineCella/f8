;; repl

(begin
  
  (define repl
	(lambda ()
	  (display ">> ")
	  (display (eval (read) (interaction-environment))) (newline)
	  (repl)))

  (display "[minimal repl]") (newline) (newline)
  (repl)
  )
