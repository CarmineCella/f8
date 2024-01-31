;; {convolution-based reverb}
;;
;; (c) 2019 www.arch-programming.org
;;

(load "sound.scm")
(load "maths.scm")

(print "read IR...")
(def a (openwav16 "Concertgebouw-m.wav" "input")) 
(def ir (first (readwav16 a))) ; by default reads all file
(closewav16 a)
(print "done" (nl))

(print "read signal...")
(def a (openwav16 "anechoic1.wav" "input")) 
(def sig (first (readwav16 a))) ; by default reads all file
(closewav16 a)
(print "done" (nl))

(print "convolving...")
(def c (conv ir sig 1))
(print "done" (nl))

(print "saving data...")
(def a (openwav16 "test.wav" "output"))
(print (writewav16 a 1 c) " samples written" (nl))
(closewav16 a)

;; eof
