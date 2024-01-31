;; {ticks}
;;
;; (c) 2019 www.arch-programming.org
;;

(load "system.scm")

(def {tic} (ticks))
(prn "current ticks " tic (nl))
(def {toc} (ticks))
(prn "current ticks " toc (nl))
(prn "ticks elapsed " (- toc tic) (nl) (nl))
(test {<> toc tic}{1}) ; they should be different