;; {wave io}
;;
;; (c) 2019 www.arch-programming.org
;;

(load "sound.scm")
(load "maths.scm")

(def {sr} 44100)
(def {ch} 2)

(def {freq} 440)

(def {time} (map (\{x}{/ x sr}) (linspace 0 1024 1)))

(def {sig1} (.sin (map (\{x}{* 2 pi freq x}) time)))
(def {sig2} (.cos (map (\{x}{* 2 pi (/ freq 2) x}) time)))

(prn "write wave data...")
(def {a} (openwav16 "test.wav" "output" sr ch))
(prn (writewav16 a ch sig1 sig2) " samples written" (nl))
(closewav16 a)

(prn "read wave data...")
(def {a} (openwav16 "test.wav" "input" sr ch)) 
(def {sigr} (readwav16 a)) ; by default reads all file
(closewav16 a)

(prn "write again read data...")
(def {a} (openwav16 "test2.wav" "output" sr ch))
(prn (writewav16 a ch (first sigr) (second sigr))op " samples written" (nl))
(closewav16 a)

;; eof
