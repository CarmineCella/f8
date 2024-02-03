;; {tests for function scoping}
;;
;; (c) 2019 www.arch-programming.org
;;

(load "core.scm")

(def {b} 5)

(function {static_foo} 
	{+ b 5}
)

(function {static_bar} {
	do 
		(def {b} 2)
		(static_foo)
	}
) 

(dynamic {dynamic_foo} 
	{+ b 5}
)

(function {dynamic_bar} {
	do 
		(def {b} 2)
		(dynamic_foo)
	}
) 
 
 (prn "--- static vs dynamic scope ---" (nl) (nl))

(test {static_foo}{10})
(test {static_bar}{10})

(test {dynamic_foo}{10})
(test {dynamic_bar}{7})

(prn (nl) "ALL TESTS PASSED" (nl) (nl))