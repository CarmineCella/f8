# --------------------------------
# The f8 (fate) scripting language
# --------------------------------
#
# Example: communication with Max via OSC strings
#
# (c) www.carminecella.com
#
source "stdlib.tcl"

puts "open the Max patch in this folder..."

schedule {
    udpsend "127.0.0.1" 10000 "/osc/dur 3" true
    udpsend "127.0.0.1" 10000 "/osc/freq 440" true
} 1000 0
schedule (udpsend "127.0.0.1" 10000 "/osc/freq 330" true) 2000 0
schedule (udpsend "127.0.0.1" 10000 "/osc/freq 220" true) 5000 0

set i 0
udpsend "127.0.0.1" 10000 "/osc/dur 1" true
while (< i 50) {
    set freq (+ 220 (birandn 100 1))
    set arg (tostr "/osc/freq " freq)
    set del 200
    if (> i 25) (= del 100)
    schedule (udpsend "127.0.0.1" 10000 arg true) del 0
    set i (+ i 1)
}

# eof
