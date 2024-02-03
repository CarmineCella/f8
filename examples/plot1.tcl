;; {plot}
;;
;; (c) 2019 www.arch-programming.org
;;

(load "plotting.scm")
(load "maths.scm")

(def {a} (opensvg "test.svg" 512 512))

(def {s0} (.cos (map (\{x}{/ x 32 }) (linspace 0 256 1))))
(def {s1} (.sin (map (\{x}{/ x 16 }) (linspace 0 128 1))))
(def {s2} (.sqrt (linspace 0 1024 1)))
(def {s3} (map (\{x}{* x x})(linspace 0 1024 1)))
(polyline a s0 Aqua)
(polyline a s1 Blue)
(polyline a s2 Red)
(polyline a s3 Green)
(grid a 0 1024 -1 1)
(legend a "cosine" Aqua "sine" Blue "sqrt" Red "pow2" Green)

(title a "This is a plot!")

(closesvg a)