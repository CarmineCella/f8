;; {plot}
;;
;; (c) 2019 www.arch-programming.org
;;

(load "plotting.scm")
(load "maths.scm")

(def {a} (map (\{x}{* x (birandn 30)}) (ones 64)))
(def {b} (map (\{x}{* x (birandn 30)}) (ones 64)))

(def {s0} (.cos (map (\{x}{/ x 32 }) (linspace 0 256 1))))


(def {g} (opensvg "scatter.svg" 512 512))
(scatter g a b Blue)
(polyline g s0 Red)
(title g "Scatter plot")
(closesvg g)


;; eof
