source "stdlib.tcl"


set a 20
set b 30
set c 40

if (> a b) {

} else {
    if (> a c) {
        
    } else {
        puts "cond 3\n"
    }
}

proc (inc a) {
    updef a (+ a 1)
}

inc a
puts a "\n"

# if a {

# } elseif b {

# } elseif c {

# } else {

# }
