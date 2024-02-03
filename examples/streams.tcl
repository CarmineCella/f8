;; {streams and filesystem}
;;
;; (c) 2019 www.arch-programming.org
;;

(load "system.scm")

(prn "current directory " (tail (tail (dirlist "."))) (nl)) ; without . and ..
(prn "stats of first file " (filestat (first (dirlist "."))) (nl))

(prn "writing data to file..." (nl))
(def {astream} (openstream "test.txt" "output" "text"))
(if (not (isgood astream)) (throw "invalid output stream"))
(writestream astream "this text " {should be written} " " (tostr (join 1)) " times")
(closestream astream)

(prn "reading back data = ")
(def {bstream} (openstream  "test.txt" "input" "text"))
(if (not (isgood bstream)) (throw "invalid input stream"))
(function {readall x} {
	if (not (isgood x))
		{}
		{do
			(prn (readstream x))
			(readall x)
		}

})

(readall bstream) (prn (nl))

(prn "rewind stream and read again = ")
(rwndstream bstream) 
(readall bstream)(prn (nl))
(closestream bstream)


; (prn (nl))