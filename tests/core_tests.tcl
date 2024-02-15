# --------------------------------
# The f8 (fate) scripting language
# --------------------------------
#
# Core tests
#
# (c) www.carminecella.com
#

source "stdlib.tcl"

puts "\n" "--- builtin operators ---\n"
test '(list 1 2 3 4) '(1 2 3 4)
test '(lappend (list) 1 2 '(3 4)) '(1 2 (3 4))
test '(lhead '(a b c)) '(a)
test '(car '(1 b c)) '1
test '(car ()) (list)
test '(lhead ()) '(())
test '(cdr '())'()
test '(cdr '(a b c))'(b c)
test '(cdr '()) (list)
test '(if 1 1)'1
test '(if 0 1) '()
test '(if 0 1 else 2) '2
set dummy 21
test '(! dummy 22) '22
test '(eq 1 1) '1
test '(eq 1 2) '0
test '(eq 1 dummy) '0
test '(eq dummy dummy) '1
test '(eq '(1 2 3)'(a b c)) '0
test '(eq '(a b c)'(a b c)) '1
test '(eq '(a b c)'(a b)) '0
test '(eq (list) ()) '1
test '(eq (car '(a b c)) 'a) '1
test '(eq + +) '1
test '(eq + -) '0
test '(eq "alpha" "alpha") '1
test '(eq "alpha" "beta") '0
test '(catch (throw '(except1))) '(except1)
test '(catch (throw "except2")) '"except2"
test '(info 'exists 'dummy) '(1)
test '(info 'exists 'not_defined) '(0)
unset 'dummy
test '(unset 'dummy) '0
test '(info 'exists 'dummy) '(0)
test '(info 'typeof '(a b c)) '(list)
test '(info 'typeof '(1 2)) '(list)
test '(info 'typeof '()) '(list)
test '(info 'typeof 3) '(numeric)
test '(info 'typeof *) '(operator)
test '(info 'typeof (\'(x)'(+ x x))) '(lambda)
test '(info 'typeof "hallo") '(string)
test '(info 'typeof (car '(a))) '(symbol)
test  '(+ 1 2 (- 3 4 (* 5 6 (/ 5 3)))) '-48
test '(- 1 2 3) '-4
test '(neg 3) '-3
test '(< 1 2 3 4) '1
test '(<= 1 2 3 4) '1
test '(< 1 2 0 4) '0
test '(> 1 2 3 4) '0
test '(>= 1 2 3 4) '0
test '(> 4 4 3 2) '0
test '(>= 4 4 2) '1
test '(tostr "hallo " "world!") '"hallo world!"
test '(string 'range "hallo world!" 2 5) '"llo w"
test '(string 'find "hallo world!" "lo") '3
test '(string 'regex "1231" "^([0-9])[0-9]") '("12" "1")
test '(string 'regex  "I am looking for GeeksForGeeks articles" "Geek[a-zA-Z]+")'("GeeksForGeeks")
test '(string 'regex "GeeksForGeeks" "(Geek)(.*)") '("GeeksForGeeks" "Geek" "sForGeeks")

puts "\n" "---  atoms ---" "\n"
test '(eval true) '1
test '(eval false) '0
test '(eval '(+ 1 2)) '3

puts "\n" "---  lists ---" "\n"
test '(llength '(a b c d)) '4
test '(llength '()) '0
test '(llength '(0)) '1

test '(lindex '(a b c d) 2) 'c
test '(lindex  '(a b c d) 1) 'b

test '(llast '(1 2 3 4)) '4
test '(llast '(a b c d)) 'd

test '(ltake '(1 a 2 b 3 c) 2) '(1 a)
test '(ltake '(1 a 2 b 3 c) -1) '()
test '(ltake '(1 a 2 b 3 c) 10) '(1 a 2 b 3 c)

test '(ldrop '(1 a 2 b 3 c) 2)  '(2 b 3 c)
test '(ldrop '(1 a 2 b 3 c) -1) '(1 a 2 b 3 c)
test '(ldrop '(1 a 2 b 3 c) 10) '()

test '(lsplit '(a b 2 3) 2) '((a b) (2 3))
test '(lsplit '(a b 2 3) 3) '((a b 2) (3))
test '(lsplit '(a b 2 3) -3) '(() (a b 2 3))
test '(lsplit '(a b 2 3) 6) '((a b 2 3) ())

test '(lreverse '(a b 2 c)) '(c 2 b a)
test '(lreverse '()) '()

test '(match 'a '(1 2 a b)) '(2)
test '(match 'z '(1 2 a b)) '()
test '(match 1 '(1 2 a b)) '(0)

test '(elem 'b '(1 2 a b)) '1
test '(elem 'h '(1 2 a b)) '0
test '(elem 2 '(1 2 a b)) '1

test '(zip '(1 2 3) '(a b c)) '((1 a) (2 b) (3 c))

puts "\n" "---  higher order operators ---""\n"
test '(map neg '(1 2 3 4)) '(-1 -2 -3 -4)
test '(map (\(x)(* x x)) '(2 3 4 5)) '(4 9 16 25)

test '(filter (\(x)(<= x 2)) '(1 2 3 4)) '(1 2)
test '(filter (\(x)(> x 2)) '(1 2 3 4)) '(3 4)
test '(filter (\(x)(<= x 2)) '(12 22 3 4)) '()

# test '(unpack + '(1 2 3 4)) '(10)
# test '(unpack * '(1 2 3 4)) '(24)

test '(foldl + 0 '(1 2 3 4)) '10
test '(foldl * 1 '(1 2 3 4)) '24

puts "\n" "--- logical operators ---" "\n"
test '(not 0) '1
test '(not 1) '0
test '(or 0 1) '1
test '(or 1 0) '1
test '(or 0 0) '0
test '(or 1 1 ) '1
test '(and 0 1) '0
test '(and 1 0) '0
test '(and 0 0) '0
test '(and 1 1 ) '1
test '(<> '(1 2) '(1 2)) '0
test '(<> '(a b c) '(a b c)) '0
test '(<> '(a b c) '(a b 1)) '1
test '(<> () ()) '0

puts "\n" "ALL TESTS PASSED" "\n" "\n"


