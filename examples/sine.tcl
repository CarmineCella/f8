;; Binary I/O examples: writing a sinusoid as 32 floats
;;

(begin
  (define dur 5) ; sec
  (define sr 44100)
  (define freq 440)
  (define samples (* sr dur))
  (define st (/ 1 sr))
  (define sig 0)
  
  (for i 0 samples 1
	   (begin
		 (define v 0)
		 (set! v (sin (* 2 PI freq (/ i sr)))) 
		 (set! sig (cons v sig))
		 ))

  (display (length sig))
  (define f (open-output-file "sine.pcm"))
  (define b (write-floats sig f))
  (close-output-port f)

  (display b) (display " bytes written") (newline))
)
