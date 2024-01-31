; (load "../../src/stdlib.scm")

; (filter (lambda (x) (>= x 0)) '(-3 -2 0 2 4))

; (foldl - 0 '(1 2 3 4))

; (map (lambda (x) (/ (+ 5 x) (* 10 x))) (noise 10))
; (map (lambda (x) (* x 10)) '(1 2 3))
; (define (flip f a b) (f b a))

; (sqrt (* (sum (map square (map (lambda (x)(- x mu)) a))) normal))

if (> 0 3) (print "hallo") (print 30)

if (< 0 3) {
    print "hallo"
    print "world"
} {
    def a 30
    print (+ a 30)
}
