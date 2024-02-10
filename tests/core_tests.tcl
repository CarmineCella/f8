# --------------------------------
# The f8 (fate) scripting language
# --------------------------------
#
# Core tests
#
# (c) www.carminecella.com
#

source "stdlib.tcl"

puts "\n" "--- builtin operators ---" "\n"

test '(list 1 2 3 4)'(1 2 3 4)
test '(lappend (list) 1 2 '(3 4))'(1 2 3 4)
test '(lappend 1 2 3 4)'(1)
test '(lappend 1)'(1)
test '(head '(a b c))'(a)
test '(car '(1 b c))'(1)
test '(car ()) (list)
test '(head ())'()
test '(cdr '())'()
test '(cdr '(a b c))'(b c)
test '(cdr '()) (list)
test '(if 1 1)'(1)
test '(if 0 1) '()
test '(if 0 1 else 2) '(2)
set dummy 21
test '(! dummy 22) '(22)
test '(eq 1 1) '(1)
test '(eq 1 2) '(0)
test '(eq 1 dummy) '(0)
test '(eq dummy dummy) '(1)
test '(eq '(1 2 3)'(a b c)) '(0)
test '(eq '(a b c)'(a b c)) '(1)
test '(eq '(a b c)'(a b)) '(0)
test '(eq (list) ()) '(1)
test '(eq (car '(a b c)) 'a) '(1)
test '(eq + +) '(1)
test '(eq + -) '(0)
test '(eq "alpha" "alpha") '(1)
test '(eq "alpha" "beta") '(0)
test '(catch (throw '(except1))) '(except1)
test '(catch (throw "except2")) '("except2")
test '(info 'exists 'dummy) '(1)
test '(info 'exists 'not_defined) '(0)
test '(unset 'dummy) '(1)
test '(unset 'dummy) '(0)
test '(info 'exists 'dummy) '(0)
test '(info 'typeof '(a b c)) '(list)
test '(info 'typeof '(1 2)) '(list)
test '(info 'typeof '()) '(list)
test '(info 'typeof 3) '(numeric)
test '(info 'typeof *) '(operator)
test '(info 'typeof (\'(x)'(+ x x))) '(lambda)
test '(info 'typeof "hallo") '(string)
test '(info 'typeof (car '(a))) '(symbol)
test  '(+ 1 2 (- 3 4 (* 5 6 (/ 5 3)))) '(-48)
test '(- 1 2 3) '(-4)
test '(neg 3) '(-3)
test '(< 1 2 3 4) '(1)
test '(<= 1 2 3 4) '(1)
test '(< 1 2 0 4) '(0)
test '(> 1 2 3 4) '(0)
test '(>= 1 2 3 4) '(0)
test '(> 4 4 3 2) '(0)
test '(>= 4 4 2) '(1)
# test '(tostr "hallo " "world!")'("hallo world!"))
# test '(substr "hallo world!" 2 5)'("llo w"))
# test '(find "lo" "hallo world!" )'(3))
# test '(regex "^(\d)\d" "1231")'("12" "1"))
# test '(regex "Geek[a-zA-Z]+" "I am looking for GeeksForGeeks articles")'("GeeksForGeeks"))
# test '(regex "(Geek)(.*)" "GeeksForGeeks") '("GeeksForGeeks" "Geek" "sForGeeks"))

# puts "\n" "---  atoms ---" "\n")
# test '(lappend true)'(1))
# test '(lappend false)'(0))

# puts  "\n" "---  fundamental operators ---" "\n")
# test '(eval '(+ 1 2))'(3))
# test '(let '(do (def '(hhh) 123) (+ hhh hhh)))'(246))
# test '(assoc '(hhh))'())

# puts "\n" "---  listections ---" "\n")
# test '(len '(a b c d))'(4))
# test '(len '())'(0))
# test '(len '(0))'(1))

# test '(nth 2 '(a b c d))'(c))
# test '(nth -1 '(a b c d))'(a))
# test '(nth 20 '(a b c d))'())

# test '(last '(1 2 3 4))'(4))
# test '(last '(a b c d))'(d))

# test '(take 2 '(1 a 2 b 3 c))'(1 a))
# test '(take -1 '(1 a 2 b 3 c))'())
# test '(take 10 '(1 a 2 b 3 c))'(1 a 2 b 3 c))

# test '(drop 2 '(1 a 2 b 3 c))'(2 b 3 c))
# test '(drop -1 '(1 a 2 b 3 c))'(1 a 2 b 3 c))
# test '(drop 10 '(1 a 2 b 3 c))'())

# test '(split 2 '(a b 2 3))'('(a b) '(2 3)))
# test '(split 3 '(a b 2 3))'('(a b 2) '(3)))
# test '(split -3 '(a b 2 3))'('() '(a b 2 3)))
# test '(split 6 '(a b 2 3)'('(a b 2 3 '()'()))))

# test '(match '(a)'(1 2 a b))'(a b))
# test '(match '(z)'(1 2 a b))'())
# test '(match 1 '(1 2 a b))'(1 2 a b))

# test '(elem '(b) '(1 2 a b))'(1))
# test '(elem '(h) '(1 2 a b))'(0))
# test '(elem 2 '(1 2 a b))'(1))

# test '(reverse '(a b 2 c))'(c 2 b a))
# test '(reverse '())'())

# test '(zip '(1 2 3)'(a b c)) '('(1 a 2) '(b 3 c)))

# puts "\n" "---  higher order operators ---""\n")
# test '(map - '(1 2 3 4))'(-1 -2 -3 -4))
# test '(map (\'(x)'(* x x)) '(2 3 4 5))'(4 9 16 25))

# test '(filter (\'(x)'(<= x 2)) '(1 2 3 4)) '(1 2))
# test '(filter (\'(x)'(> x 2)) '(1 2 3 4)) '(3 4))
# test '(filter (\'(x)'(<= x 2)) '(12 22 3 4)) '())

# test '(unpack + '(1 2 3 4))'(10))
# test '(unpack * '(1 2 3 4))'(24))
# test '(pack head 1 2 3 4) '(1))
# test '(pack cdr 1 2 3 4) '(2 3 4))
# test '(pack len 1 2 '(a) 3)'(4))

# test '(foldl + 0 '(1 2 3 4))'(10))
# test '(foldl * 1 '(1 2 3 4))'(24))

# puts "\n" "--- logical operators ---" "\n")
# test '(not 0)'(1))
# test '(not 0)'(1))
# test '(or 0 1)'(1))
# test '(or 1 0)'(1))
# test '(or 0 0)'(0))
# test '(or 1 1 )'(1))
# test '(and 0 1)'(0))
# test '(and 1 0)'(0))
# test '(and 0 0)'(0))
# test '(and 1 1 )'(1))
# test '(<> '(1 2)'(1 2))'(0))
# test '(<> '(a b c)'(a b c))'(0))
# test '(<> '(a b c)'(a b 1))'(1))

# puts "\n""--- conditionals ---" "\n")

# (function '(day-name x) '(
#   case x
#     '(1 "Monday")
#     '(2 "Tuesday")
#     '(3 "Wednesday")
#     '(4 "Thursday")
#     '(5 "Friday")
#     '(6 "Saturday")
#     '(7 "Sunday")
# ))

# test '(day-name 1)'("Monday"))
# test '(day-name 4)'("Thursday"))
# test '(day-name 7)'("Sunday"))

puts "\n" "ALL TESTS PASSED" "\n" "\n"


